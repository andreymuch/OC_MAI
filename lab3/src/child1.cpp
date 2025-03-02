#include <windows.h>
#include <iostream>
#include <cctype>
#include <locale>

#define BUFFER_SIZE 1024

int main() {


    Sleep(500);
    HANDLE hMapFile;
    char* pBuf;
    std::locale loc("ru_RU.UTF-8");

    hMapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, "SharedMemory");
    if (hMapFile == NULL) {
        std::cerr << "Не удалось открыть отображаемый файл: " << GetLastError() << std::endl;
        return 1;
    }

    pBuf = (char*)MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, BUFFER_SIZE);
    if (pBuf == NULL) {
        std::cerr << "Не удалось отобразить файл: " << GetLastError() << std::endl;
        CloseHandle(hMapFile);
        return 1;
    }
    for (int i = 0; pBuf[i]; i++) {
        pBuf[i] = toupper(pBuf[i], loc);
    }

    UnmapViewOfFile(pBuf);
    CloseHandle(hMapFile);

    return 0;
}
