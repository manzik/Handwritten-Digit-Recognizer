#pragma once

#include <iostream>
#include <conio.h>
#include <fstream>

#include "config.h"
#include "utils.h"

using namespace std;

void readMnist(std::string imagesPath, string labelsPath, double*** pixelsDataArr, int** labelsArr, int* count);