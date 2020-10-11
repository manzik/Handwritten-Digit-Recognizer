

// main.cpp : Defines the entry point for the console application.
//

#include <iostream>
#include <conio.h>
#include <time.h>
#include <fstream>
#include <string>
#include <math.h>
#include <iomanip>
#include <sstream>
#include <chrono>
#include "lib/mnist/mnist_reader.hpp"
#include "lib/mnist/mnist_reader_less.hpp"

#include "config.h"

#include "utils.h"
#include "english_dataset_reader.h"
#include "farsi_dataset_reader.h"
#include "MLP.cpp"

using namespace std;


// Neural netowrk's processing mode
enum NNProcessMode { Train, Test };

// Forward declerations.
double GetRandomConnectionWeight(int size);
void processPrintFinalStatistics(MultilayerPerceptron* neuralNet, double*** pixelsData, int** labelsArr, int count, int testPercentage);
void processNNUsingData(MultilayerPerceptron* neuralNet, 
		double* inputNeurons, int inputNeuronsCount, int outputAnswer, 
		double learningRate, NNProcessMode processMode);
void readEnglishDataset(double*** pixelsDataArr, int** labelsArr, int* count);
void readPersianDataset(double*** pixelsDataArr, int** labelsArr, int* count);

MultilayerPerceptron mynet;
int seenSamples = 0, totalSamples;

int main()
{

	cout << "Initializing" << "\n";

	// For reproducibility
	srand(43);

	mynet = MultilayerPerceptron(STARTING_LEARNING_RATE);
	int NeuronLayersCount = 4;
	int NeuronLayers[4] = { 28 * 28, 512, 512, 10 };
	int sum = 0;
	for (int i = 0; i < NeuronLayersCount; i++)
	{
		sum += NeuronLayers[i];
	}
	cout << "Making a neural network with " << sum << " connections" << "\n";


	mynet.CreateNeurons(sum);

	int index = 0;

	for (int i = 1; i < NeuronLayersCount; i++)
	{
		for (int j = index; j < index + NeuronLayers[i - 1]; j++)
		{
			for (int k = index + NeuronLayers[i - 1]; k < index + NeuronLayers[i - 1] + NeuronLayers[i]; k++)
			{
				double inp = GetRandomConnectionWeight(10);

				mynet.ConnectNeurons(j, k, inp);
			}
		}
		index += NeuronLayers[i - 1];
	}

	cout << "Initialized" << "\n";

	int processEpochs = PROCESS_EPOCHS;

	double** pixelsDataArr;
	int* labelsArr;
	int count;

	cout << "Training using the dataset for the " << LANG_STR << " language\n";
	cout << "Reading the dataset..\n";

#if TRAIN_LANG == ENGLISH
	readEnglishDataset(&pixelsDataArr, &labelsArr, &count);
#elif TRAIN_LANG == PERSIAN
	readPersianDataset(&pixelsDataArr, &labelsArr, &count);
#endif

	cout << "Done reading the dataset (loaded " << count << " samples).." << "\n";
	cout << "Training.." << "\n";

	auto train_start = std::chrono::high_resolution_clock::now();

	totalSamples = processEpochs * count;
	cout << "Learning rate formula (with domain " << 0 << " to " << totalSamples << "): " << STARTING_LEARNING_RATE << "*" << EXP_DECAY_BASE
	     << "^(-" << getExponentialDecayGetConstantK(totalSamples) << "*x)";

	for (int epoch = 0; epoch < processEpochs; epoch++)
	{
		cout << "\n=============== EPOCH " << (epoch + 1) << "/" << processEpochs << " ===============";
		for (int i = 0; i < count; i++)
		{
			int currentSampleInd = epoch * count + i;
			double learningRate = getExponentialDecayLearningRate(currentSampleInd, totalSamples);
			bool shouldTrainOnData = TEST_PERCENTAGE == 0 || seenSamples % 100 >= TEST_PERCENTAGE;
			NNProcessMode processMode = shouldTrainOnData ? Train : Test;
			processNNUsingData(&mynet, pixelsDataArr[i], 28 * 28, labelsArr[i], learningRate, processMode);
		}
	}

	auto train_end = std::chrono::high_resolution_clock::now();

	double trainingElapsedSeconds = std::chrono::duration<double, std::milli>(train_end - train_start).count() / 1000;

	cout << "\nDone training. Elapsed training seconds: " << trainingElapsedSeconds;

	cout << "Testing..";

	processPrintFinalStatistics(&mynet, &pixelsDataArr, &labelsArr, count, TEST_PERCENTAGE);

	// Clear up memory
	for (int i = 0; i < count; i++)
	{
		delete[] pixelsDataArr[i];
	}
	delete[] pixelsDataArr;

	delete[] labelsArr;

	_getch();

	return 0;
}

// Does operations on the neural network. Could be either backpropagation (for trainingdata) or just feedfoward.
// Will also calculate and print debug info and save checkpoints when needed
Stats iterationTrainStats, iterationTestStats;
void processNNUsingData(MultilayerPerceptron *neuralNet, double* inputNeurons, int inputNeuronsCount, int outputAnswer, double learningRate, NNProcessMode processMode)
{
	neuralNet->LearningRate = learningRate;

	neuralNet->ClearUp();
	for (int i = 0; i < inputNeuronsCount; i++)
	{
		neuralNet->SetNeuronValue(i, inputNeurons[i]);
	}

	neuralNet->ComputeOutput();
	double currentMSE = 0;
	for (int i = 0; i < 10; i++)
	{
		currentMSE += pow((i == outputAnswer ? 1 : 0) - neuralNet->GetNeuronValue(neuralNet->NeuronsSize - 10 + i), 2) / 10;
	}

	double maxNeuronVal = -1;
	int indOfMaxNeuronVal = 0;
	for (int i = 0; i < 10; i++)
	{
		double neuronValue = neuralNet->GetNeuronValue(neuralNet->NeuronsSize - 10 + i);
		if (neuronValue > maxNeuronVal)
		{
			maxNeuronVal = neuronValue;
			indOfMaxNeuronVal = i;
		}
	}
	bool correctPrediction = indOfMaxNeuronVal == outputAnswer;

	if (processMode == Train)
	{
		// TRAIN

		// Update training stats
		iterationTrainStats.incCumulativeMSE(currentMSE);
		iterationTrainStats.incCount();
		if (correctPrediction)
			iterationTrainStats.incCorrect();

		// Perform backpropagation and cleanup the backpropagation resources
		int dosslen = 10;
		DesiredOutput** desiredOutputs = new DesiredOutput * [dosslen];
		for (int i = 0; i < dosslen; i++)
		{
			desiredOutputs[i] = new DesiredOutput(neuralNet->NeuronsSize - 10 + i, ((i == outputAnswer) ? 1 : 0));
		}
		neuralNet->BackPropagate(desiredOutputs, dosslen);

		for (int i = 0; i < dosslen; i++)
			delete desiredOutputs[i];
		delete[] desiredOutputs;
	}
	else
	{
		// TEST

		// Update testing stats
		iterationTestStats.incCumulativeMSE(currentMSE);
		iterationTestStats.incCount();
		if (correctPrediction)
			iterationTestStats.incCorrect();
	}

	bool showingDebugMessage = seenSamples % SHOW_DEBUG_NTH_SAMPLE == 0 && seenSamples != 0 && SHOW_DEBUG;
	bool showingCheckpointMessage = seenSamples % CHECKPOINT_NTH_SAMPLE == 0 && seenSamples != 0;
	if (showingDebugMessage || showingCheckpointMessage)
		cout << "\n\n";

	// Debug printing
	if (showingDebugMessage)
	{
		const char fullNeuronChar = '@';
		const char emptyNeuronChar = '.';

		cout << "------------ DEBUG ------------\n";
		cout << "Data type: " << (processMode == Train ? "Training data" : "Testing Data") << "\n";
		cout << "Input Neurons:\n";
		for (int y = 0; y < 28; y++)
		{
			for (int x = 0; x < 28; x++)
			{
				cout << (round(inputNeurons[x + 28 * y]) == 1 ? fullNeuronChar : emptyNeuronChar) << " ";
			}
			cout << "\n";
		}

		cout << "Output Neurons:\n";
		for (int i = 0; i < 10; i++)
		{
			cout << neuralNet->GetNeuronValue(neuralNet->NeuronsSize - 10 + i) << " ";
		}
		cout << "\n" << "Ground Truth, NN Answer (confidence):\t" << outputAnswer << ", " << indOfMaxNeuronVal << " (" << maxNeuronVal << ")" << "\n";
	}

	if (showingCheckpointMessage)
	{
		string netdata = "[";
		for (int i = 0; i < neuralNet->ConnectionsSize; i++)
		{
			double weight = (neuralNet->Connections[i])->Weight;
			std::stringstream ss;
			ss << std::setprecision( std::numeric_limits<double>::digits10+2);
			ss << weight;
			netdata += ss.str() + ",";
		}

		netdata = netdata.substr(0, netdata.length() - 1) + "]";

		std::ofstream outfile;

		outfile.open("netdata.txt", std::ios_base::trunc);
		outfile << netdata;

		outfile.close();

		int iterationTrainSamplesCount = iterationTrainStats.getCount(), iterationTestSamplesCount = iterationTestStats.getCount();
		cout << "------------------------------\n"
			<< "Saved Neural Network Weights. \n"
			<< "Total samples Seen/Remaining: " << seenSamples << "/" << totalSamples << "\n"
			<< "Learning Rate: " << neuralNet->LearningRate << "\n";

		cout << "------------ TRAIN ------------\n";
		cout << "Used: " << iterationTrainSamplesCount << "\n";
		cout << "Loss (MSE): " << iterationTrainStats.getMSE() << "\n"
			<< "Accuracy: " << iterationTrainStats.getAccuracy() * 100 << "%\n";

		cout << "------------ TEST ------------\n";
		cout << "Used: " << iterationTestSamplesCount << "\n";
		cout << "Loss (MSE): " << iterationTestStats.getMSE() << "\n"
			<< "Accuracy: " << iterationTestStats.getAccuracy() * 100 << "%\n";
		cout << "------------------------------\n";


		//cin >> neuralNet->LearningRate;
		iterationTrainStats.reset();
		iterationTestStats.reset();
	}
	seenSamples++;
}

// Calculates overall statistics of the model's performance based on the whole dataset for each train and test proportion of the dataset.
void processPrintFinalStatistics(MultilayerPerceptron* neuralNet, double*** pixelsData, int** labelsArr, int count, int testPercentage)
{
	double testMSE = 0, testAccuracy = 0,
		trainMSE = 0, trainAccuracy = 0;
	int testDataCount = 0, trainDataCount = 0;
	int testCorrectAnswers = 0, trainCorrectAnswers = 0;
	for (int i = 0; i < count; i++)
	{
		bool isTestData = TEST_PERCENTAGE != 0 && seenSamples % 100 < TEST_PERCENTAGE;

		neuralNet->ClearUp();
		for (int j = 0; j < 28 * 28; j++)
		{
			double inputPixel = (*pixelsData)[i][j];
			neuralNet->SetNeuronValue(j, inputPixel);
		}
		neuralNet->ComputeOutput();

		int groundTruth = (*labelsArr)[i];

		double currentMSE = 0;
		for (int j = 0; j < 10; j++)
		{
			currentMSE += pow((j == groundTruth ? 1 : 0) - neuralNet->GetNeuronValue(neuralNet->NeuronsSize - 10 + j), 2) / 10;
		}


		double maxNeuronVal = -1;
		int indOfMaxNeuronVal = 0;
		for (int j = 0; j < 10; j++)
		{
			double neuronValue = neuralNet->GetNeuronValue(neuralNet->NeuronsSize - 10 + j);
			if (neuronValue > maxNeuronVal)
			{
				maxNeuronVal = neuronValue;
				indOfMaxNeuronVal = j;
			}
		}
		bool correctPrediction = indOfMaxNeuronVal == groundTruth;

		if (isTestData)
		{
			testMSE += currentMSE;
			if (correctPrediction)
				testCorrectAnswers++;
			testDataCount++;
		}
		else
		{
			trainMSE += currentMSE;
			if (correctPrediction)
				trainCorrectAnswers++;
			trainDataCount++;
		}
	}

	testMSE /= testDataCount;
	testAccuracy = (double)testCorrectAnswers / testDataCount;

	trainMSE /= trainDataCount;
	trainAccuracy = (double)trainCorrectAnswers / trainDataCount;

	cout << "Training data MSE: " << trainMSE << "\n";
	cout << "Training data accuracy: " << trainAccuracy * 100 << "%" << "\n";
	cout << "\n";
	cout << "Testing data MSE: " << testMSE << "\n";
	cout << "Testing data accuracy: " << testAccuracy * 100 << "%" << "\n";
}