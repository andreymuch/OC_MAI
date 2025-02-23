#pragma once
#include "matrix.h"

Matrix parallelErosion(Matrix& matrix, size_t windowSize, size_t numThreads, size_t count);
Matrix  parallelDilate(Matrix& matrix, size_t windowSize, size_t numThreads, size_t count);
