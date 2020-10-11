#include <iostream>
#include "config.h"

#include "utils.h"

using namespace std;

double Stats::getMSE()
{
	return cumulativeMSE / totalCount;
}
double Stats::getAccuracy()
{
	return (double)correctCount / totalCount;
}
int Stats::getCount()
{
	return totalCount;
}
void Stats::incCumulativeMSE(double inc)
{
	Stats::cumulativeMSE += inc;
}
void Stats::incCount()
{
	Stats::totalCount++;
}
void Stats::incCorrect()
{
	Stats::correctCount++;
}
void Stats::reset()
{
	totalCount = 0;
	correctCount = 0;
	cumulativeMSE = 0;
}

void mergeData(
	double*** pixelsDataArr1, double*** pixelsDataArr2, double*** pixelsDataArrDest,
	int** labelsArr1, int** labelsArr2, int** labelsArrDest,
	int* count1, int* count2, int* countDest)
{
	*countDest = *count1 + *count2;
	*pixelsDataArrDest = new double* [*countDest];
	*labelsArrDest = new int[*countDest];

	for (int i = 0; i < *count1; i++)
	{
		(*pixelsDataArrDest)[i] = (*pixelsDataArr1)[i];
		(*labelsArrDest)[i] = (*labelsArr1)[i];
	}
	for (int i = 0; i < *count2; i++)
	{
		(*pixelsDataArrDest)[*count1 + i] = (*pixelsDataArr2)[i];
		(*labelsArrDest)[*count1 + i] = (*labelsArr2)[i];
	}
}

void randomTransformPixelsDataArr(double*** pixelsDataArr, int count)
{
	const double MINIMUM_SCALE = 0.7;
	for (int i = 0; i < count; i++)
	{
		double* newPixelsData = new double[28 * 28];
		double* oldPixelsData = (*pixelsDataArr)[i];

		int digitImageTop = 28, digitImageLeft = 28, digitImageRight = -1, digitImageBottom = -1;
		for (int y = 0; y < 28; y++)
		{
			for (int x = 0; x < 28; x++)
			{
				double pixelVal = oldPixelsData[x + y * 28];
				if (pixelVal > 0)
				{
					if (x < digitImageLeft)
						digitImageLeft = x;
					if (x > digitImageRight)
						digitImageRight = x;
					if (y < digitImageTop)
						digitImageTop = y;
					if (y > digitImageBottom)
						digitImageBottom = y;
				}
			}
		}

		int digitWidth = digitImageRight - digitImageLeft + 1;
		int digitHeight = digitImageBottom - digitImageTop + 1;

		double scale = ((double)rand() / (RAND_MAX)) * (1 - MINIMUM_SCALE) + MINIMUM_SCALE; // Set scale to random value from 0.5 to 1
		int marginX = (28 - (double)digitWidth * scale) * ((double)rand() / (RAND_MAX));
		int marginY = (28 - (double)digitHeight * scale) * ((double)rand() / (RAND_MAX));

		for (int y = 0; y < 28; y++)
		{
			for (int x = 0; x < 28; x++)
			{
				int oldPixelsX = round((double)x / scale - marginX) + digitImageLeft;
				int oldPixelsY = round((double)y / scale - marginY) + digitImageTop;
				double newPixelVal;
				if (oldPixelsX < 0 || oldPixelsY < 0 || oldPixelsX >= 28 || oldPixelsY >= 28)
					newPixelVal = 0;
				else
					newPixelVal = oldPixelsData[oldPixelsX + oldPixelsY * 28];
				newPixelsData[x + y * 28] = newPixelVal;
			}
		}

		(*pixelsDataArr)[i] = newPixelsData;
		delete[] oldPixelsData;
	}
}

// Random weight generation function: Uses He Normal initialization (He et al.).
// Could possibly be replaced with Xavier weight Initialization.
double GetRandomConnectionWeight(int size)
{
	// RandomNormal not used for now. Could possibly be used for Xavier weight initialization.
	double RandomNormal = cos((double)2 * 3.1415926 * ((double)rand() / (RAND_MAX))) * sqrt((double)-2 * log(((double)rand() / (RAND_MAX))));

	return (double)(((double)rand() / (RAND_MAX)) * 2 - 1) * ((double)sqrt((double)1 / (double)size));
}

double getExponentialDecayGetConstantK(int totalSamples)
{
	/*
	*
	* Exponential-ish decay (base could be anything):
	* lr(t) = lr0 * (base)^(-kt) where t = currentSample and lr0 = STARTING_LEARNING_RATE
	*
	* We need to find k such that lr(totalSamples) = ENDING_LEARNING_RATE and lr(0) = STARTING_LEARNING_RATE
	* Solving:
	* ENDING_LEARNING_RATE = STARTING_LEARNING_RATE * (base)^(-k * totalSamples)
	* ENDING_LEARNING_RATE / STARTING_LEARNING_RATE = (base)^(-k * totalSamples)
	* log(ENDING_LEARNING_RATE / STARTING_LEARNING_RATE) = log((base)^(-k * totalSamples))
	* log(ENDING_LEARNING_RATE / STARTING_LEARNING_RATE) = (-k * totalSamples) * log(base)
	* k = - log(ENDING_LEARNING_RATE / STARTING_LEARNING_RATE) / (totalSamples * log(base))
	*
	*/
	double k = -log((double)ENDING_LEARNING_RATE / STARTING_LEARNING_RATE) / (totalSamples * log(EXP_DECAY_BASE));

	return k;
}

double getExponentialDecayLearningRate(int currentSample, int totalSamples)
{
	double k = getExponentialDecayGetConstantK(totalSamples);
	return STARTING_LEARNING_RATE * pow(EXP_DECAY_BASE, -k * currentSample);
}