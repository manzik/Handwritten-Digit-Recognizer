

// Persian Handwritten Digit Recognizer.cpp : Defines the entry point for the console application.
//


#include <windows.h>
//#include "stdafx.h"
//#include <atlimage.h>
//#include <atlstr.h>
#include <minwindef.h>
#include <iostream>
#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <chrono>
#include <thread>
#include <fstream>
#include <string> 
#include <iostream>
#include <string>
#include <iomanip>
#include <math.h>
#include <sstream>
#include "GNN.cpp"

using namespace std;

double GenerateRandNumber()
{
	return 10 * (((double)rand() / (RAND_MAX)) * 2 - 1);
}
//double M_PII = ;
double GetRandomValueForNeuron(int size)
{
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

GeneralNeuralNetwork mygnn;
double predc = 0;
CSample *Samples;
byte imgType;
int level = 0;
int idx = 0;
//CImage img;

unsigned long CurAddr = 0;
void ReadData(string FileName);
void ReadRamFile(void* Buffer, int ByteCount);
enum ImageType { itBinary = 0, itGray = 1 };
void ReadRamFile(void* Buffer, int ByteCount)
{
	CopyMemory(Buffer, (void*)CurAddr, ByteCount);
	CurAddr += ByteCount;
};
int counterd = 0;
void Teach(double *data, int datacount, int answer)
{
	//mygnn.LearningRate *= 0.99995;
	int inputlen = datacount;

	mygnn.ClearUp();
	for (int i = 0; i < inputlen; i++)
	{
		mygnn.SetNeuronValue(i, data[i]);
	}


	mygnn.ComputeOutput();
	/*
	double TotalSum=0;
	for (int i = 0; i < 10; i++)
			{
				TotalSum+=exp(mygnn.GetNeuronValue(mygnn.NeuronsSize - 10 + i));
			}
	*/
	int maxind = 0, maxval = 0;
	for (int i = 0; i < 10; i++)
	{
		if (maxval < mygnn.GetNeuronValue(mygnn.NeuronsSize - 10 + i))
		{
			maxval = mygnn.GetNeuronValue(mygnn.NeuronsSize - 10 + i);
			maxind = i;
		}
		
	}
	predc += abs((maxind != answer) ? 1 : 0) / 1;
	if (counterd % 100 == 0)
	{
		for (int i = 0; i < 10; i++)
		{
			cout << mygnn.GetNeuronValue(mygnn.NeuronsSize - 10 + i) << " ";

			//cout << ((maxind == answer) ? "True" : "False") << " " << counterd << "\n";
		}
		cout << answer << "\n";
	}


	int dosslen = 10;

	DesiredOutput **dos = new DesiredOutput*[dosslen];

	for (int i = 0; i < dosslen; i++)
	{
		dos[i] = new DesiredOutput(mygnn.NeuronsSize - 10 + i, ((i == answer) ? 1 : 0));
	}

	mygnn.BackPropagate(dos, dosslen, false);

	//mygnn.LearningRate *= 0.99993;

	if (counterd % 1000 == 0)
	{
		string netdata = "[";
		for (int i = 0; i < mygnn.ConnectionsSize; i++)
		{
			netdata += std::to_string((mygnn.Connections[i])->Weight) + ",";
		}

		netdata = netdata.substr(0, netdata.length() - 1) + "]";

		std::ofstream outfile;

		outfile.open("netdata.txt", std::ios_base::trunc);//std::ios_base::app
		outfile << netdata;

		cout << (predc / 1000) * 100 << " " << counterd << "\n";
		//cin >> mygnn.LearningRate;
		predc = 0;
	}
	counterd++;
}



void ReadData(string FileName)
{
	counterd = 0;
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
		cout << "AAAAAAAA";
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



	};//i
	if (imgType == itGray)
		level = 1;
	else
		level = 255;
	UnmapViewOfFile(pBase);
	CloseHandle(hFile);
}




int main()
{
	cout << "Initializing" << "\n";

	srand(time(NULL));

	mygnn = GeneralNeuralNetwork();
	int NeuronLayersCount = 4;
	int NeuronLayers[4] = { 28 * 28,512,512,10 };
	int sum = 0;
	for (int i = 0; i < NeuronLayersCount; i++)
	{
		sum += NeuronLayers[i];
	}
	cout << sum << "\n";


	mygnn.CreateNeurons(sum);

	int index = 0;

	for (int i = 1; i < NeuronLayersCount; i++)
	{
		for (int j = index; j < index + NeuronLayers[i - 1]; j++)
		{
			for (int k = index + NeuronLayers[i - 1]; k < index + NeuronLayers[i - 1] + NeuronLayers[i]; k++)
			{
				double inp = (double)GetRandomValueForNeuron(10);
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
				//cout << GetRandomValueForNeuron(NeuronLayers[i]) << "\n\n";
				//


				mygnn.ConnectNeurons(j, k, inp);

			}
		}
		index += NeuronLayers[i - 1];
	}

	cout << "Initialized" << "\n";
	ReadData("Train 60000.cdb");
	ReadData("Train 60000.cdb");
	ReadData("Train 60000.cdb");
	ReadData("Train 60000.cdb");
	ReadData("Train 60000.cdb");
	ReadData("Train 60000.cdb");
	std::this_thread::sleep_for(std::chrono::milliseconds(10000));



	return 0;
}
