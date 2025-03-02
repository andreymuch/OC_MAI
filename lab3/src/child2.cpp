#include <windows.h>
#include <iostream>
#include <cstring>

#define BUFFER_SIZE 1024

void remove_duplicate_spaces(char* str) {
    int count = 0;
    bool last_was_space = false;

    for (int i = 0; str[i]; i++) {
        if (str[i] != ' ') {
            str[count++] = str[i];
            last_was_space = false;
        } else if (!last_was_space) {
            str[count++] = str[i];
            last_was_space = true;
        }
    }

    str[count] = '\0';
}

int main() {
    Sleep(1500);
    HANDLE hMapFile;
    char* pBuf;

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

    HANDLE hChild1Process = *reinterpret_cast<HANDLE*>(pBuf + BUFFER_SIZE - sizeof(HANDLE));

    WaitForSingleObject(hChild1Process, INFINITE);
    remove_duplicate_spaces(pBuf);
    UnmapViewOfFile(pBuf);
    CloseHandle(hMapFile);
    CloseHandle(hChild1Process);

    return 0;
}
