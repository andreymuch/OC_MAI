#include "filters.h"
#include <algorithm>
#include <limits>
#include <windows.h>

#undef min
#undef max

struct ThreadData {
    Matrix* result;
    Matrix* matrix;
    size_t startRow;
    size_t endRow;
    size_t windowSize;
    HANDLE hEvent;
    HANDLE hEventDone;
};

DWORD WINAPI ErosionThreadProc(LPVOID lpParam) {
    ThreadData* data = static_cast<ThreadData*>(lpParam);
    size_t padding = data->windowSize / 2;

    for(int iter=0;;iter++) {
        WaitForSingleObject(data->hEvent, INFINITE);
        Matrix* src = iter%2 == 0 ? data->matrix : data->result;
        Matrix* dst = iter%2 == 0 ? data->result : data->matrix;
        for (size_t i = data->startRow; i < data->endRow; ++i) {
            for (size_t j = 0; j < dst->cols(); ++j) {
                double minVal = std::numeric_limits<double>::max();
                for (size_t wi = 0; wi < data->windowSize; ++wi) {
                    for (size_t wj = 0; wj < data->windowSize; ++wj) {
                        size_t ni = (i + wi < padding) ? 0 : (i + wi - padding);
                        size_t nj = (j + wj < padding) ? 0 : (j + wj - padding);
                        if (ni >= src->rows()) ni = src->rows() - 1;
                        if (nj >= src->cols()) nj = src->cols() - 1;
                        minVal = std::min(minVal, src->at(ni, nj));
                    }
                }
                dst->at(i, j) = minVal;
            }
        }
        SetEvent(data->hEventDone);
        ResetEvent(data->hEvent);
    }

    return 0;
}

Matrix parallelErosion(Matrix& matrix, size_t windowSize, size_t numThreads, size_t count) {
    size_t rows = matrix.rows();
    size_t cols = matrix.cols();

    Matrix result(rows, cols);

    HANDLE* threadHandles = new HANDLE[numThreads];
    ThreadData* threadData = new ThreadData[numThreads];
    HANDLE* doneEvents = new HANDLE[numThreads];

    size_t rowsPerThread = rows / numThreads;
    for (size_t i = 0; i < numThreads; ++i) {
        threadData[i].result = &result;
        threadData[i].matrix = &matrix;
        threadData[i].startRow = i * rowsPerThread;
        threadData[i].endRow = (i == numThreads - 1) ? rows : (i + 1) * rowsPerThread;
        threadData[i].windowSize = windowSize;
        threadData[i].hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
        threadData[i].hEventDone = CreateEvent(NULL, TRUE, FALSE, NULL);
        doneEvents[i] = threadData[i].hEventDone;

        threadHandles[i] = CreateThread(NULL, 0, ErosionThreadProc, &threadData[i], 0, NULL);
    }

    for (size_t i = 0; i < count; ++i) {
        for (size_t j = 0; j < numThreads; ++j) {
            ResetEvent(doneEvents[j]);
            SetEvent(threadData[j].hEvent);
        }
        WaitForMultipleObjects(numThreads, doneEvents, TRUE, INFINITE);
    }


    for (size_t j = 0; j < numThreads; ++j) {
        TerminateThread(threadHandles[j],0);
        CloseHandle(doneEvents[j]);
        CloseHandle(threadData[j].hEvent);
    }
    delete[] threadHandles;
    delete[] threadData;

    return count%2==0 ? matrix : result;
}

DWORD WINAPI DilateThreadProc(LPVOID lpParam) {
    ThreadData* data = static_cast<ThreadData*>(lpParam);
    size_t padding = data->windowSize / 2;

    for(int iter=0;;iter++) {
        WaitForSingleObject(data->hEvent, INFINITE);
        Matrix* src = iter%2 == 0 ? data->matrix : data->result;
        Matrix* dst = iter%2 == 0 ? data->result : data->matrix;
        for (size_t i = data->startRow; i < data->endRow; ++i) {
            for (size_t j = 0; j < dst->cols(); ++j) {
                double maxVal = std::numeric_limits<double>::min();
                for (size_t wi = 0; wi < data->windowSize; ++wi) {
                    for (size_t wj = 0; wj < data->windowSize; ++wj) {
                        size_t ni = (i + wi < padding) ? 0 : (i + wi - padding);
                        size_t nj = (j + wj < padding) ? 0 : (j + wj - padding);
                        if (ni >= src->rows()) ni = src->rows() - 1;
                        if (nj >= src->cols()) nj = src->cols() - 1;
                        maxVal = std::max(maxVal, src->at(ni, nj));
                    }
                }
                dst->at(i, j) = maxVal;
            }
        }
        SetEvent(data->hEventDone);
        ResetEvent(data->hEvent);
    }

    return 0;
}

Matrix parallelDilate(Matrix& matrix, size_t windowSize, size_t numThreads, size_t count) {
    size_t rows = matrix.rows();
    size_t cols = matrix.cols();

    Matrix result(rows, cols);

    HANDLE* threadHandles = new HANDLE[numThreads];
    ThreadData* threadData = new ThreadData[numThreads];
    HANDLE* doneEvents = new HANDLE[numThreads];

    size_t rowsPerThread = rows / numThreads;
    for (size_t i = 0; i < numThreads; ++i) {
        threadData[i].result = &result;
        threadData[i].matrix = &matrix;
        threadData[i].startRow = i * rowsPerThread;
        threadData[i].endRow = (i == numThreads - 1) ? rows : (i + 1) * rowsPerThread;
        threadData[i].windowSize = windowSize;
        threadData[i].hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
        threadData[i].hEventDone = CreateEvent(NULL, TRUE, FALSE, NULL);
        doneEvents[i] = threadData[i].hEventDone;

        threadHandles[i] = CreateThread(NULL, 0, DilateThreadProc, &threadData[i], 0, NULL);
    }

    for (size_t i = 0; i < count; ++i) {
        for (size_t j = 0; j < numThreads; ++j) {
            ResetEvent(doneEvents[j]);
            SetEvent(threadData[j].hEvent);
        }
        WaitForMultipleObjects(numThreads, doneEvents, TRUE, INFINITE);
    }

    for (size_t j = 0; j < numThreads; ++j) {
        TerminateThread(threadHandles[j],0);
        CloseHandle(doneEvents[j]);
        CloseHandle(threadData[j].hEvent);
    }
    delete[] threadHandles;
    delete[] threadData;

    return count%2==0 ? matrix : result;
}