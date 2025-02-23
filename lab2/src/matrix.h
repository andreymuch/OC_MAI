#pragma once
#include <vector>
#include <iostream>


class Matrix {
public:
    Matrix(size_t rows, size_t cols);
    double& at(size_t row, size_t col);
    const double& at(size_t row, size_t col) const;
    size_t rows() const { return rows_; }
    size_t cols() const { return cols_; }
    void print() const {
        for (size_t i = 0; i < rows_; ++i) {
            for (size_t j = 0; j < cols_; ++j) {
                std::cout << at(i, j) << " ";
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }

private:
    size_t rows_, cols_;
    std::vector<double> data_;
};