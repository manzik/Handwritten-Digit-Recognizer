

// main.cpp : Defines the entry point for the console application.
//

#include <iostream>
#include <conio.h>
#include <string>
#include <math.h>
#include <iomanip>
#include <sstream>
#include <chrono>
#include <codecvt>

#include "config.h"

#include "lib/mnist/mnist_reader.hpp"
#include "lib/mnist/mnist_reader_less.hpp"
#include "lib/lzstring/lzstring.h"

#include "utils.h"
#include "english_dataset_reader.h"
#include "persian_dataset_reader.h"
#include "mlp.cpp"

using namespace std;

// Neural netowrk's processing mode
enum NNProcessMode { Train, Test };

// Forward declerations.
double GetRandomConnectionWeight(int size);
void processPrintFinalStatistics(MultilayerPerceptron* neuralNet, double*** pixelsData, int** labelsArr, int count);
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
	srand(45);

	mynet = MultilayerPerceptron(STARTING_LEARNING_RATE);
	int NeuronLayersCount = 4;
	int neuronLayers[4] = { 28 * 28, 512, 512, 10 };
	int sum = 0;
	for (int i = 0; i < NeuronLayersCount; i++)
	{
		sum += neuronLayers[i];
	}
	cout << "Making a neural network with " << sum << " connections" << "\n";


	mynet.createNeurons(sum);

	// Set connections of the network (fully-connected).
	int index = 0;
	for (int i = 1; i < NeuronLayersCount; i++) // Layer i
	{
		for (int j = index; j < index + neuronLayers[i - 1]; j++) // Neuron with ind j in in layer i-1
		{
			for (int k = index + neuronLayers[i - 1]; k < index + neuronLayers[i - 1] + neuronLayers[i]; k++) // Neuron with ind k in layer i
			{
				// Connect neuron with ind j to neuron with ind k, initializing with a random weight
				double inp = GetRandomConnectionWeight(10);
				mynet.connectNeurons(j, k, inp);
			}
		}
		index += neuronLayers[i - 1];
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

	auto trainStart = std::chrono::high_resolution_clock::now();

	totalSamples = processEpochs * count;
	cout << "Learning rate formula (with domain " << 0 << " to " << totalSamples << "): " << STARTING_LEARNING_RATE << "*" << EXP_DECAY_BASE
	     << "^(-" << getExponentialDecayGetConstantK(totalSamples) << "*x)";

	double exponentialDecayConstant = getExponentialDecayGetConstantK(totalSamples);
	for (int epoch = 0; epoch < processEpochs; epoch++)
	{
		cout << "\n=============== EPOCH " << (epoch + 1) << "/" << processEpochs << " ===============";
		for (int i = 0; i < count; i++)
		{
			int currentSampleInd = epoch * count + i; // Will be equal to sampleSeen variable. Just wanted it to be independent one
			double learningRate = getExponentialDecayLearningRate(currentSampleInd, totalSamples, exponentialDecayConstant);
			
			bool shouldTrainOnData = TEST_PERCENTAGE == 0 || currentSampleInd % 100 >= TEST_PERCENTAGE;
			NNProcessMode processMode = shouldTrainOnData ? Train : Test;
			processNNUsingData(&mynet, pixelsDataArr[i], 28 * 28, labelsArr[i], learningRate, processMode);
		}
	}

	auto trainEnd = std::chrono::high_resolution_clock::now();

	double trainingElapsedSeconds = std::chrono::duration<double, std::milli>(trainEnd - trainStart).count() / 1000;

	cout << "\nDone training. Elapsed training seconds: " << trainingElapsedSeconds << "\n";

	cout << "Testing.." << "\n";

	processPrintFinalStatistics(&mynet, &pixelsDataArr, &labelsArr, count);

	// Clear up memory. Compiler will take care of it for now.
	/*
	for (int i = 0; i < count; i++)
	{
		delete[] pixelsDataArr[i];
	}
	delete[] pixelsDataArr;

	delete[] labelsArr;
	*/
	cout << "Enter a character to exit." << "\n";
	_getch();

	return 0;
}

// convert wstring to UTF-8 string
std::string wstringToUtf8(const std::wstring& str)
{
	std::wstring_convert<std::codecvt_utf8<wchar_t>> myconv;
	return myconv.to_bytes(str);
}

// Does operations on the neural network. Could be either backpropagation (for trainingdata) or just feedfoward.
// Will also calculate and print debug info and save checkpoints when needed
Stats iterationTrainStats, iterationTestStats;
void processNNUsingData(MultilayerPerceptron *neuralNet, double* inputNeurons, int inputNeuronsCount, int outputAnswer, double learningRate, NNProcessMode processMode)
{
	neuralNet->learningRate = learningRate;

	neuralNet->cleanUp();
	for (int i = 0; i < inputNeuronsCount; i++)
	{
		neuralNet->setNeuronValue(i, inputNeurons[i]);
	}

	neuralNet->computeOutput();
	double currentMSE = 0;
	for (int i = 0; i < 10; i++)
	{
		currentMSE += pow((i == outputAnswer ? 1 : 0) - neuralNet->getNeuronValue(neuralNet->neuronsSize - 10 + i), 2) / 10;
	}

	double maxNeuronVal = -1;
	int indOfMaxNeuronVal = 0;
	for (int i = 0; i < 10; i++)
	{
		double neuronValue = neuralNet->getNeuronValue(neuralNet->neuronsSize - 10 + i);
		if (neuronValue > maxNeuronVal)
		{
			maxNeuronVal = neuronValue;
			indOfMaxNeuronVal = i;
		}
	}
	bool isCorrectPrediction = indOfMaxNeuronVal == outputAnswer;

	if (processMode == Train)
	{
		// TRAIN

		// Update training stats
		iterationTrainStats.incCumulativeMSE(currentMSE);
		iterationTrainStats.incCount();
		if (isCorrectPrediction)
			iterationTrainStats.incCorrect();

		// Perform backpropagation and cleanup the backpropagation resources
		int dosslen = 10;
		DesiredOutput** desiredOutputs = new DesiredOutput *[dosslen];
		for (int i = 0; i < dosslen; i++)
		{
			desiredOutputs[i] = new DesiredOutput(neuralNet->neuronsSize - 10 + i, ((i == outputAnswer) ? 1 : 0));
		}
		neuralNet->backPropagate(desiredOutputs, dosslen);

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
		if (isCorrectPrediction)
			iterationTestStats.incCorrect();
	}

	bool shouldShowDebugMessage = seenSamples % SHOW_DEBUG_NTH_SAMPLE == 0 && seenSamples != 0 && SHOW_DEBUG;
	bool shouldShowCheckpointMessage = seenSamples % CHECKPOINT_NTH_SAMPLE == 0 && seenSamples != 0;
	if (shouldShowDebugMessage || shouldShowCheckpointMessage)
		cout << "\n\n";

	// Debug printing
	if (shouldShowDebugMessage)
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
			cout << neuralNet->getNeuronValue(neuralNet->neuronsSize - 10 + i) << " ";
		}
		cout << "\n" << "Ground Truth, NN Answer (confidence):\t" << outputAnswer << ", " << indOfMaxNeuronVal << " (" << maxNeuronVal << ")" << "\n";

		int iterationTrainSamplesCount = iterationTrainStats.getCount(), iterationTestSamplesCount = iterationTestStats.getCount();
		cout << "------------ TRAIN ------------\n";
		cout << "Used: " << iterationTrainSamplesCount << "\n";
		cout << "Loss (MSE): " << iterationTrainStats.getMSE() << "\n"
			 << "Accuracy: " << iterationTrainStats.getAccuracy() * 100 << "%\n";

		cout << "------------ TEST ------------\n";
		cout << "Used: " << iterationTestSamplesCount << "\n";
		cout << "Loss (MSE): " << iterationTestStats.getMSE() << "\n"
			 << "Accuracy: " << iterationTestStats.getAccuracy() * 100 << "%\n";
		cout << "------------------------------\n";
		cout << "Total samples Seen/Remaining: " << seenSamples << "/" << totalSamples << "\n"
			 << "Learning Rate: " << neuralNet->learningRate << "\n"
			 << "------------------------------\n";

		//cin >> neuralNet->LearningRate;
		iterationTrainStats.reset();
		iterationTestStats.reset();
	}

	if (shouldShowCheckpointMessage)
	{
		string networkDataStr = "[";
		for (int i = 0; i < neuralNet->connections.size(); i++)
		{
			double weight = (neuralNet->connections[i])->weight;
			std::stringstream ss;
			ss << std::setprecision( std::numeric_limits<double>::digits10+2);
			ss << weight;
			networkDataStr += ss.str() + ",";
		}

		networkDataStr = networkDataStr.substr(0, networkDataStr.length() - 1) + "]";

		std::wofstream outFile;
		wstring compressedNetDataWStr = LZString::CompressToBase64(networkDataStr);

		outFile.open(PARAMETERS_SAVE_PATH, std::ios_base::binary);
		outFile << compressedNetDataWStr;

		outFile.close();

		cout << "==============================\n"
			 << "Saved Neural Network Weights. \n"
			 << "==============================\n";
	}
	seenSamples++;
}

// Calculates overall statistics of the model's performance based on the whole dataset for each train and test proportion of the dataset.
void processPrintFinalStatistics(MultilayerPerceptron* neuralNet, double*** pixelsData, int** labelsArr, int count)
{
	double testMSE = 0, testAccuracy = 0,
		trainMSE = 0, trainAccuracy = 0;
	int testDataCount = 0, trainDataCount = 0;
	int testCorrectAnswers = 0, trainCorrectAnswers = 0;
	for (int i = 0; i < count; i++)
	{
		bool isTestData = TEST_PERCENTAGE != 0 && i % 100 < TEST_PERCENTAGE;

		neuralNet->cleanUp();
		for (int j = 0; j < 28 * 28; j++)
		{
			double inputPixel = (*pixelsData)[i][j];
			neuralNet->setNeuronValue(j, inputPixel);
		}
		neuralNet->computeOutput();

		int groundTruth = (*labelsArr)[i];

		double currentMSE = 0;
		for (int j = 0; j < 10; j++)
		{
			currentMSE += pow((j == groundTruth ? 1 : 0) - neuralNet->getNeuronValue(neuralNet->neuronsSize - 10 + j), 2) / 10;
		}


		double maxNeuronVal = -1;
		int indOfMaxNeuronVal = 0;
		for (int j = 0; j < 10; j++)
		{
			double neuronValue = neuralNet->getNeuronValue(neuralNet->neuronsSize - 10 + j);
			if (neuronValue > maxNeuronVal)
			{
				maxNeuronVal = neuronValue;
				indOfMaxNeuronVal = j;
			}
		}
		bool isCorrectPrediction = indOfMaxNeuronVal == groundTruth;

		if (isTestData)
		{
			testMSE += currentMSE;
			if (isCorrectPrediction)
				testCorrectAnswers++;
			testDataCount++;
		}
		else
		{
			trainMSE += currentMSE;
			if (isCorrectPrediction)
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
