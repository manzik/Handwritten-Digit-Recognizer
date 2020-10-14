#include "english_dataset_reader.h"

using namespace std;

void readMnist(string imagesPath, string labelsPath, double*** pixelsDataArr, int** labelsArr, int* count);
void randomTransformPixelsDataArr(double*** pixelsDataArr, int count);

void readEnglishDataset(double*** pixelsDataArr, int** labelsArr, int* count)
{
	double** trainPixelsDataArr, ** testPixelsDataArr;
	int* trainLabelsArr, * testLabelsArr;
	int trainCount, testCount;

	readMnist(DATASETS_PATH "/train-images-idx3-ubyte", DATASETS_PATH"/train-labels-idx1-ubyte", &trainPixelsDataArr, &trainLabelsArr, &trainCount);
	readMnist(DATASETS_PATH "/t10k-images-idx3-ubyte", DATASETS_PATH "/t10k-labels-idx1-ubyte", &testPixelsDataArr, &testLabelsArr, &testCount);

	// Label values will be copied
	// pixelData's pointer to array of image data will be copied.
	mergeData(&trainPixelsDataArr, &testPixelsDataArr, pixelsDataArr,
		&trainLabelsArr, &testLabelsArr, labelsArr,
		&trainCount, &testCount, count);

	delete[] trainPixelsDataArr;
	delete[] testPixelsDataArr;

	delete[] trainLabelsArr;
	delete[] testLabelsArr;

	if (ENGLISH_DATASET_RANDOM_TRANSFORMATION)
		randomTransformPixelsDataArr(pixelsDataArr, *count);
}

int reverseInt(int i)
{
	unsigned char c1, c2, c3, c4;

	c1 = i & 255;
	c2 = (i >> 8) & 255;
	c3 = (i >> 16) & 255;
	c4 = (i >> 24) & 255;

	return ((int)c1 << 24) + ((int)c2 << 16) + ((int)c3 << 8) + c4;
}

// https://stackoverflow.com/a/33384846
typedef unsigned char uchar;
int* readMnistLabels(string full_path, int& number_of_labels)
{
	auto reverseInt = [](int i)
	{
		unsigned char c1, c2, c3, c4;
		c1 = i & 255, c2 = (i >> 8) & 255, c3 = (i >> 16) & 255, c4 = (i >> 24) & 255;
		return ((int)c1 << 24) + ((int)c2 << 16) + ((int)c3 << 8) + c4;
	};

	ifstream file(full_path, ios::binary);

	if (file.is_open())
	{
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
	else
	{
		throw runtime_error("Unable to open file `" + full_path + "`!");
	}
}

// https://github.com/wichtounet/mnist
void readMnist(string imagesPath, string labelsPath, double*** pixelsDataArr, int** labelsArr, int* count)
{
	ifstream file(imagesPath, ios::binary);
	if (file.is_open())
	{
		int magic_number = 0;
		int number_of_images = 0;



		int n_rows = 0;
		int n_cols = 0;
		file.read((char*)&magic_number, sizeof(magic_number));
		magic_number = reverseInt(magic_number);
		file.read((char*)&number_of_images, sizeof(number_of_images));
		number_of_images = reverseInt(number_of_images);
		file.read((char*)&n_rows, sizeof(n_rows));
		n_rows = reverseInt(n_rows);
		file.read((char*)&n_cols, sizeof(n_cols));
		n_cols = reverseInt(n_cols);


		(*pixelsDataArr) = new double* [number_of_images];
		(*count) = number_of_images;
		for (int i = 0; i < number_of_images; i++)
		{
			double* pixelsData = new double[28 * 28];
			memset(pixelsData, 0, 28 * 28);
			for (int r = 0; r < n_rows; r++)
			{
				for (int c = 0; c < n_cols; c++)
				{
					unsigned char temp = 0;
					file.read((char*)&temp, sizeof(temp));
					pixelsData[r * n_rows + c] = ((double)temp / 255) > 0 ? 1 : 0;
				}
			}

			(*pixelsDataArr)[i] = pixelsData;
		}
	}
	else
	{
		throw runtime_error("Unable to open file `" + imagesPath + "`!");
	}

	int dataCount = -1, imageSize = 28;
	int* labels = readMnistLabels(labelsPath, dataCount);
	*labelsArr = labels;
}

// Scales and moves the digits to a random location
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

		int biggestDimension = digitWidth > digitHeight ? digitWidth : digitHeight;

		// Find the scale that makes the digit fit the 28*28 grid of values.
		double maxScale = digitWidth > digitHeight ? ((double)28 / digitWidth) : ((double)28 / digitHeight);

		double scale = ((double)rand() / (RAND_MAX)) * (maxScale - MINIMUM_SCALE) + MINIMUM_SCALE; // Set scale to random value in the minimum and maximum value range
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