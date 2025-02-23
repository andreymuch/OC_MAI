#include <gtest/gtest.h>
#include "../src/filters.h"

TEST(PadMatrixTest, StandardTest) {
    Matrix inputMatrix(3, 3);
    inputMatrix.at(0, 0) = 1.0;
    inputMatrix.at(0, 1) = 2.0;
    inputMatrix.at(0, 2) = 3.0;
    inputMatrix.at(1, 0) = 4.0;
    inputMatrix.at(1, 1) = 5.0;
    inputMatrix.at(1, 2) = 6.0;
    inputMatrix.at(2, 0) = 7.0;
    inputMatrix.at(2, 1) = 8.0;
    inputMatrix.at(2, 2) = 9.0;

    Matrix expectedOutputMatrix(5, 5);
    expectedOutputMatrix.at(0, 0) = 1.0;
    expectedOutputMatrix.at(0, 1) = 1.0;
    expectedOutputMatrix.at(0, 2) = 2.0;
    expectedOutputMatrix.at(0, 3) = 3.0;
    expectedOutputMatrix.at(0, 4) = 3.0;
    expectedOutputMatrix.at(1, 0) = 1.0;
    expectedOutputMatrix.at(1, 1) = 1.0;
    expectedOutputMatrix.at(1, 2) = 2.0;
    expectedOutputMatrix.at(1, 3) = 3.0;
    expectedOutputMatrix.at(1, 4) = 3.0;
    expectedOutputMatrix.at(2, 0) = 4.0;
    expectedOutputMatrix.at(2, 1) = 4.0;
    expectedOutputMatrix.at(2, 2) = 5.0;
    expectedOutputMatrix.at(2, 3) = 6.0;
    expectedOutputMatrix.at(2, 4) = 6.0;
    expectedOutputMatrix.at(3, 0) = 7.0;
    expectedOutputMatrix.at(3, 1) = 7.0;
    expectedOutputMatrix.at(3, 2) = 8.0;
    expectedOutputMatrix.at(3, 3) = 9.0;
    expectedOutputMatrix.at(3, 4) = 9.0;
    expectedOutputMatrix.at(4, 0) = 7.0;
    expectedOutputMatrix.at(4, 1) = 7.0;
    expectedOutputMatrix.at(4, 2) = 8.0;
    expectedOutputMatrix.at(4, 3) = 9.0;
    expectedOutputMatrix.at(4, 4) = 9.0;

    Matrix outputMatrix = padMatrix(inputMatrix, 3);

    for (size_t i = 0; i < expectedOutputMatrix.rows(); ++i) {
        for (size_t j = 0; j < expectedOutputMatrix.cols(); ++j) {
            EXPECT_DOUBLE_EQ(expectedOutputMatrix.at(i, j), outputMatrix.at(i, j));
        }
    }
}