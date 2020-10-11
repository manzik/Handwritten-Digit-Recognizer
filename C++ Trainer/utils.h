#pragma once

class Stats
{
	int totalCount = 0;
	int correctCount = 0;
	double cumulativeMSE = 0; // SE: Squared Error
public:
	double getMSE();
	double getAccuracy();
	int getCount();
	void incCumulativeMSE(double inc);
	void incCount();
	void incCorrect();
	void reset();
};

void mergeData(
	double*** pixelsDataArr1, double*** pixelsDataArr2, double*** pixelsDataArrDest,
	int** labelsArr1, int** labelsArr2, int** labelsArrDest,
	int* count1, int* count2, int* countDest);

void randomTransformPixelsDataArr(double*** pixelsDataArr, int count);

double getExponentialDecayGetConstantK(int totalSamples);
double getExponentialDecayLearningRate(int currentSample, int totalSamples);