#include "matrix.h"
#include <iostream>

Matrix::Matrix(size_t rows, size_t cols) : rows_(rows), cols_(cols), data_(rows * cols) {}

double& Matrix::at(size_t row, size_t col) {
    return data_[row * cols_ + col];
}

const double& Matrix::at(size_t row, size_t col) const {
    return data_[row * cols_ + col];
}