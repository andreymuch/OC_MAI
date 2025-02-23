#include <iostream>
#include <windows.h>
#include <string>
#include <chrono>
#include "matrix.h"
#include "filters.h"

#include <random>
int main(int argc, char* argv[]) { 
    Matrix inputMatrix(30, 30);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 10.0); // Generate random numbers between 0.0 and 10.0

    for (int i = 0; i < 30; ++i) {
        for (int j = 0; j < 30; ++j) {
            inputMatrix.at(i, j) = dis(gen);
        }
    }
    inputMatrix.print();

    int count, threads;
    std::cout << "Enter count ";
    std::cin >> count;
    std::cout << "Enter threads: ";
    std::cin >> threads;

    auto start = std::chrono::high_resolution_clock::now();

    Matrix outputMatrix1 = parallelErosion(inputMatrix, 3, threads / 2, count);
    Matrix outputMatrix2 = parallelDilate (inputMatrix, 3, threads / 2, count);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    outputMatrix1.print();
    outputMatrix2.print();
    std::cout << "Thread completion time: " << duration.count() << " microseconds" << std::endl;

    return 0;
}

