#include <windows.h>
#include <fstream>


#include "config.h"
#include "utils.h"

using namespace std;

void readHoda(string fileName, double*** pixelsDataArr, int** labelsArr, int* count);

void readPersianDataset(double*** pixelsDataArr, int** labelsArr, int* count)
{
	double** trainPixelsDataArr, ** testPixelsDataArr;
	int* trainLabelsArr, * testLabelsArr;
	int trainCount, testCount;

	readHoda(DATASETS_PATH "/Train 60000.cdb", &trainPixelsDataArr, &trainLabelsArr, &trainCount);
	readHoda(DATASETS_PATH "/Test 20000.cdb", &testPixelsDataArr, &testLabelsArr, &testCount);

	// Label values will be copied
	// pixelData's pointer to array of image data will be copied.
	mergeData(&trainPixelsDataArr, &testPixelsDataArr, pixelsDataArr,
		&trainLabelsArr, &testLabelsArr, labelsArr,
		&trainCount, &testCount, count);

	delete[] trainPixelsDataArr;
	delete[] testPixelsDataArr;

	delete[] trainLabelsArr;
	delete[] testLabelsArr;
}


// Function from samples of http://farsiocr.ir/%D9%85%D8%AC%D9%85%D9%88%D8%B9%D9%87-%D8%AF%D8%A7%D8%AF%D9%87/%D9%85%D8%AC%D9%85%D9%88%D8%B9%D9%87-%D8%A7%D8%B1%D9%82%D8%A7%D9%85-%D8%AF%D8%B3%D8%AA%D9%86%D9%88%DB%8C%D8%B3-%D9%87%D8%AF%DB%8C/
void readHoda(string fileName, double*** pixelsDataArr, int** labelsArr, int* count)
{
	string FileName = fileName;

	unsigned long CurAddr = 0;
	auto ReadRamFile = [&](void* Buffer, int ByteCount)
	{
		CopyMemory(Buffer, (void*)CurAddr, ByteCount); CurAddr += ByteCount;
	};

	struct CSample
	{
		unsigned char w;
		unsigned char h;
		int lable;
		BYTE* Data;
	};
	enum ImageType { itBinary = 0, itGray = 1 };
	double predc = 0;
	CSample* Samples;
	byte imgType;
	int level = 0;
	int idx = 0;

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

	*pixelsDataArr = new double* [TotalRec];
	*labelsArr = new int[TotalRec];
	*count = TotalRec;

	for (i = 0; i < TotalRec; i++)
	{
		CSample& Sample = Samples[i];
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
		Samples[i].Data = new BYTE[H * W];

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
							Samples[i].Data[y * W + x + counter] = 0;//Background
						else
							Samples[i].Data[y * W + x + counter] = 1;//ForeGround
						x++;
					};
					bWhite = !bWhite;//black white black white ...
					counter += WBcount;
				}
			};
		}
		else//GrayScale mode
		{
			ReadRamFile(Samples[i].Data, W * H);
		};


		double* pixelsData = new double[28 * 28];
		memset(pixelsData, 0, 28 * 28);

		double rscale = 1;
		if (Sample.h > 28 || Sample.w > 28)
		{
			rscale = (max(Sample.w, Sample.h) / (((double)rand() / (RAND_MAX) * 14 + 14)));
		}
		int marginx = ((double)rand() / (RAND_MAX)) * (28 - Sample.w / rscale);
		int marginy = ((double)rand() / (RAND_MAX)) * (28 - Sample.h / rscale);

		for (int x = 0; x < Sample.w; x += rscale)
		{
			for (int y = 0; y < Sample.h; y += rscale)
			{
				pixelsData[(int)((round(y / rscale) + marginy) * 28 + marginx + round(x / rscale))] = Sample.Data[(int)(floor(y) * Sample.w + floor(x))];
			}
		}

		(*pixelsDataArr)[i] = pixelsData;
		(*labelsArr)[i] = Sample.lable;

	};//i

	if (imgType == itGray)
		level = 1;
	else
		level = 255;
	UnmapViewOfFile(pBase);
	CloseHandle(hFile);
}