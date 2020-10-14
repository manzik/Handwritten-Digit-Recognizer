#pragma once

#include <windows.h>
#include <fstream>
#include <iostream>

#include "config.h"
#include "utils.h"

using namespace std;

void readPersianDataset(double*** pixelsDataArr, int** labelsArr, int* count);