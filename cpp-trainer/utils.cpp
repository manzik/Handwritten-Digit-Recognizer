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

// Merges two arrays of pixelData and labels
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

// Random weight generation function: Uses He Normal initialization (He et al.).
// Could possibly be replaced with Xavier weight Initialization.
double GetRandomConnectionWeight(int size)
{
	// RandomNormal not used for now. Could possibly be used for Xavier weight initialization.
	double RandomNormal = cos((double)2 * 3.1415926 * ((double)rand() / (RAND_MAX))) * sqrt((double)-2 * log(((double)rand() / (RAND_MAX))));

	return (double)(((double)rand() / (RAND_MAX)) * 2 - 1) * ((double)sqrt((double)1 / (double)size));
}

// Get's the constant k for exponential decay
// while meeting necessary needs (meeting ENDING_LEARNING_RATE in the beginning and STARTING_LEARNING_RATE in the end)
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

// Returns learning rate for exponential decay based on the current sample index
double getExponentialDecayLearningRate(int currentSample, int totalSamples, double k)
{
	return STARTING_LEARNING_RATE * pow(EXP_DECAY_BASE, -k * currentSample);
}
double getExponentialDecayLearningRate(int currentSample, int totalSamples)
{
	double k = getExponentialDecayGetConstantK(totalSamples);
	return getExponentialDecayLearningRate(currentSample, totalSamples, k);
}