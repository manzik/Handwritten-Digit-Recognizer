

// Persian Handwritten Digit Recognizer.cpp : Defines the entry point for the console application.
//


#include <windows.h>
#include <iostream>
#include <conio.h>
#include <time.h>
#include <fstream>
#include <string>
#include <math.h>

#include "MLP.cpp"
#include "include/mnist/mnist_reader.hpp"
#include "include/mnist/mnist_reader_less.hpp"

using namespace std;

// Could be "english" "persian"
// Set to train for digits in the desired language
#define TRAIN_LANG "persian"

double GenerateRandNumber()
{
	return 10 * (((double)rand() / (RAND_MAX)) * 2 - 1);
}

// Random weight generation function: Uses He Normal initialization (He et al.).
// Could possibly be replaced with Xavier weight Initialization.
double GetRandomConnectionWeight(int size)
{
	// RandomNormal not used for now. Could possibly be used for Xavier weight initialization.
	double RandomNormal = cos((double)2 * 3.1415926*((double)rand() / (RAND_MAX)))*sqrt((double)-2 * log(((double)rand() / (RAND_MAX))));

	return (double)(((double)rand() / (RAND_MAX)) * 2 - 1)*((double)sqrt((double)1 / (double)size));
}


struct CSample
{
	unsigned char w;
	unsigned char h;
	int lable;
	BYTE* Data;
};

MultilayerPerceptron mynet;
double predc = 0;
CSample *Samples;
byte imgType;
int level = 0;
int idx = 0;
//CImage img;

unsigned long CurAddr = 0;
void readAndTrainPersian(string FileName);
void ReadRamFile(void* Buffer, int ByteCount);
enum ImageType { itBinary = 0, itGray = 1 };
void ReadRamFile(void* Buffer, int ByteCount)
{
	CopyMemory(Buffer, (void*)CurAddr, ByteCount);
	CurAddr += ByteCount;
};

int seenSamples = 0;
int iterationSamples = 0;
double cumulativeMSE = 0;
int correctCount = 0;
void Teach(double *data, int datacount, int answer)
{
	// Uncomment for decaying learning rate. Said to improve the network.
	//mynet.LearningRate *= 0.99995;
	int inputlen = datacount;

	mynet.ClearUp();
	for (int i = 0; i < inputlen; i++)
	{
		mynet.SetNeuronValue(i, data[i]);
	}

	mynet.ComputeOutput();
	double currentMSE = 0;
	for (int i = 0; i < 10; i++)
	{

		currentMSE += pow((i == answer ? 1 : 0) - mynet.GetNeuronValue(mynet.NeuronsSize - 10 + i), 2) / 10;
	}
	cumulativeMSE += currentMSE;

	// Uncomment to see all output neuron values predictions.
	/*
	if (seenSamples % 100 == 0)
	{
		for (int i = 0; i < 10; i++)
		{
			cout << mynet.GetNeuronValue(mynet.NeuronsSize - 10 + i) << " ";
		}
		cout << answer << "\n";
	}
	*/
	
	bool correctClassPrediction = false;
	double maxNeuronVal = -1;
	int indOfMaxNeuronVal = 0;
	for (int i = 0; i < 10; i++)
	{
		double neuronValue = mynet.GetNeuronValue(mynet.NeuronsSize - 10 + i);
		if (neuronValue > maxNeuronVal)
		{
			maxNeuronVal = neuronValue;
			indOfMaxNeuronVal = i;
		}
	}
	bool correctPrediction = indOfMaxNeuronVal == answer;
	if (correctPrediction)
		correctCount++;


	int dosslen = 10;

	DesiredOutput **desiredOutputs = new DesiredOutput*[dosslen];

	for (int i = 0; i < dosslen; i++)
	{
		desiredOutputs[i] = new DesiredOutput(mynet.NeuronsSize - 10 + i, ((i == answer) ? 1 : 0));
	}

	mynet.BackPropagate(desiredOutputs, dosslen, false);

	if (seenSamples % 1000 == 0 && seenSamples != 0)
	{
		string netdata = "[";
		for (int i = 0; i < mynet.ConnectionsSize; i++)
		{
			netdata += std::to_string((mynet.Connections[i])->Weight) + ",";
		}

		netdata = netdata.substr(0, netdata.length() - 1) + "]";

		std::ofstream outfile;

		outfile.open("netdata.txt", std::ios_base::trunc);
		outfile << netdata;

		outfile.close();

		int wrongCount = iterationSamples - correctCount;
		cout << "Seen " << iterationSamples << " samples. Saved weights." << "\n";
		cout << "Mean Squared Error: " << (cumulativeMSE / iterationSamples) * 100 << "\n"
			 << "Accuracy: " << (double)correctCount / iterationSamples * 100 << "%\n"
			 << "Samples Seen: " << seenSamples << "\n";
		//cin >> mynet.LearningRate;
		cumulativeMSE = 0;
		correctCount = 0;
		iterationSamples = 0;
	}
	seenSamples++;
	iterationSamples++;

	for (int i = 0; i < dosslen; i++)
		delete desiredOutputs[i];
	delete[] desiredOutputs;
}


// Function from samples of http://farsiocr.ir/%D9%85%D8%AC%D9%85%D9%88%D8%B9%D9%87-%D8%AF%D8%A7%D8%AF%D9%87/%D9%85%D8%AC%D9%85%D9%88%D8%B9%D9%87-%D8%A7%D8%B1%D9%82%D8%A7%D9%85-%D8%AF%D8%B3%D8%AA%D9%86%D9%88%DB%8C%D8%B3-%D9%87%D8%AF%DB%8C/
void readAndTrainPersian(string FileName)
{
	HANDLE hFile, hMap;
	void* pBase;
	BYTE d, m, W, H, x, y, StartByte, counter, WBcount;
	WORD yy, ByteCount;
	DWORD* LetterCount;
	char Comments[256];
	DWORD TotalRec;
	bool normal, bWhite;
	int i;

	hFile = CreateFile((LPCSTR)FileName.c_str(), GENERIC_READ,
		FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, 0);
	if (hFile == NULL)
	{
		hFile = 0;
		MessageBox(HWND(NULL), (LPCSTR)"File Can not be loaded", (LPCSTR)"error", MB_ICONERROR);
		return;
	};

	hMap = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
	pBase = MapViewOfFile(hMap, FILE_MAP_READ, 0, 0, 0);
	CloseHandle(hMap);
	hMap = 0;
	CurAddr = unsigned long(pBase);

	//read private header
	ReadRamFile(&yy, 2);
	ReadRamFile(&m, 1);
	ReadRamFile(&d, 1);
	ReadRamFile(&W, 1);
	ReadRamFile(&H, 1);
	ReadRamFile(&TotalRec, 4);

	LetterCount = new DWORD[128];
	ZeroMemory(LetterCount, 128 * sizeof(DWORD));

	ReadRamFile(LetterCount, 4 * 128);
	ReadRamFile(&imgType, 1);
	if (imgType != itGray)
		imgType = itBinary;
	ReadRamFile(Comments, 256);
	if ((W > 0) && (H > 0))
		normal = true;
	else
		normal = false;

	CurAddr = unsigned long(pBase) + 1024;//bypass 1024 bytes header
	Samples = new CSample[TotalRec];
	for (i = 0; i < TotalRec; i++)
	{
		CSample &Sample = Samples[i];
		ReadRamFile(&StartByte, 1);//must be 0xff
		ReadRamFile(&Samples[i].lable, 1);
		if (!normal)
		{
			ReadRamFile(&W, 1);
			ReadRamFile(&H, 1);
		};
		ReadRamFile(&ByteCount, 2);
		Samples[i].h = H;
		Samples[i].w = W;
		Samples[i].Data = new BYTE[H*W];

		if (imgType == itBinary)
		{
			for (y = 0; y < H; y++)
			{
				bWhite = true;
				counter = 0;
				while (counter < W)
				{
					ReadRamFile(&WBcount, 1);
					x = 0;
					while (x < WBcount)
					{
						if (bWhite)
							Samples[i].Data[y*W + x + counter] = 0;//Background
						else
							Samples[i].Data[y*W + x + counter] = 1;//ForeGround
						x++;
					};
					bWhite = !bWhite;//black white black white ...
					counter += WBcount;
				}
			};
		}
		else//GrayScale mode
		{
			ReadRamFile(Samples[i].Data, W*H);
		};
		double rscale = 1;
		if (Sample.h > 28 || Sample.w > 28)
		{
			rscale = (max(Sample.w, Sample.h) / (((double)rand() / (RAND_MAX) * 14 + 14)));
		}
		int marginx = ((double)rand() / (RAND_MAX))*(28 - Sample.w / rscale);
		int marginy = ((double)rand() / (RAND_MAX))*(28 - Sample.h / rscale);


		double *TData = new double[28 * 28];

		for (int x = 0; x < Sample.w; x += rscale)
		{
			for (int y = 0; y < Sample.h; y += rscale)
			{
				TData[(int)((round(y / rscale) + marginy) * 28 + marginx + round(x / rscale))] = Sample.Data[(int)(floor(y)*Sample.w + floor(x))];
			}
		}
		int num = Sample.lable;
		//cout << Sample.lable << " "; 
		Teach(TData, 28 * 28, num);	

		delete[] TData;


	};//i
	if (imgType == itGray)
		level = 1;
	else
		level = 255;
	UnmapViewOfFile(pBase);
	CloseHandle(hFile);
}



typedef unsigned char uchar;
// https://stackoverflow.com/a/33384846
int* read_mnist_labels(string full_path, int& number_of_labels) {
	auto reverseInt = [](int i) {
		unsigned char c1, c2, c3, c4;
		c1 = i & 255, c2 = (i >> 8) & 255, c3 = (i >> 16) & 255, c4 = (i >> 24) & 255;
		return ((int)c1 << 24) + ((int)c2 << 16) + ((int)c3 << 8) + c4;
	};

	ifstream file(full_path, ios::binary);

	if (file.is_open()) {
		int magic_number = 0;
		file.read((char*)&magic_number, sizeof(magic_number));
		magic_number = reverseInt(magic_number);

		if (magic_number != 2049) throw runtime_error("Invalid MNIST label file!");

		file.read((char*)&number_of_labels, sizeof(number_of_labels)), number_of_labels = reverseInt(number_of_labels);

		uchar* _char_dataset = new uchar[number_of_labels];
		for (int i = 0; i < number_of_labels; i++) {
			file.read((char*)&_char_dataset[i], 1);
		}

		int* _dataset = new int[number_of_labels];
		for (int i = 0; i < number_of_labels; i++) {
			_dataset[i] = _char_dataset[i];
		}
		return _dataset;
	}
	else {
		throw runtime_error("Unable to open file `" + full_path + "`!");
	}
}

void readAndTrainEnglish()
{
	int dataCount = -1, imageSize = 28;
	string labels_path = "train-labels-idx1-ubyte";

	int* labels = read_mnist_labels(labels_path, dataCount);
	cout << dataCount;
	mnist::MNIST_dataset<std::vector, std::vector<uint8_t>, uint8_t> images =
	mnist::read_dataset<std::vector, std::vector, uint8_t, uint8_t>(".");

	vector<vector<uchar>> training_images = images.training_images;

	std::cout << "Nbr of training images = " << training_images.size() << std::endl;
	_getch();
}


int main()
{
	string langMode = TRAIN_LANG;

	cout << "Initializing" << "\n";

	srand(time(NULL));

	mynet = MultilayerPerceptron();
	int NeuronLayersCount = 4;
	int NeuronLayers[4] = { 28 * 28, 400, 400, 10 };
	int sum = 0;
	for (int i = 0; i < NeuronLayersCount; i++)
	{
		sum += NeuronLayers[i];
	}
	cout << sum << "\n";


	mynet.CreateNeurons(sum);

	int index = 0;

	for (int i = 1; i < NeuronLayersCount; i++)
	{
		for (int j = index; j < index + NeuronLayers[i - 1]; j++)
		{
			for (int k = index + NeuronLayers[i - 1]; k < index + NeuronLayers[i - 1] + NeuronLayers[i]; k++)
			{
				double inp = (double)GetRandomConnectionWeight(10);
				/*
				if (k == index + NeuronLayers[i - 1])
				{
					cout << i;
					cout << " ";
					cout << j;
					cout << " ";
					cout << k;
					cout << "\n";
					cout << inp << "\n";
				}
				*/
				//cout << GetRandomConnectionWeight(NeuronLayers[i]) << "\n\n";
				//


				mynet.ConnectNeurons(j, k, inp);

			}
		}
		index += NeuronLayers[i - 1];
	}

	cout << "Initialized" << "\n";

	int trainEpochsPersian = 10, trainEpochsEnglish = 10;
	int trainEpochs = TRAIN_LANG == "english" ? trainEpochsEnglish : trainEpochsPersian;
	
	if (langMode == "persian")
		for (int i = 0; i < trainEpochsPersian; i++)
			readAndTrainPersian("Train 60000.cdb");
	else if (langMode == "english")
		for (int i = 0; i < trainEpochsPersian; i++)
			readAndTrainEnglish();
	else throw new std::exception("Type of language not defined");



	return 0;
}
