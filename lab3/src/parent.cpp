#include <windows.h>
#include <iostream>
#include <string>

#define BUFFER_SIZE 1024

int main() {
    HANDLE hFile;   
    HANDLE hMapFile;
    char* pBuf;
    PROCESS_INFORMATION pi1, pi2;
    STARTUPINFO si1, si2;
    std::string input;

    hFile = CreateFile(
        "lab3file",              // Имя файла
        GENERIC_READ | GENERIC_WRITE, // Доступ на чтение и запись
        0,                       // Файл не может быть разделён между процессами
        NULL,                    // Атрибуты безопасности по умолчанию
        CREATE_ALWAYS,           // Создаёт новый файл или перезаписывает существующий
        FILE_ATTRIBUTE_NORMAL,   // Обычный файл
        NULL                     // Нет шаблона для файла
    );

    if (hFile == INVALID_HANDLE_VALUE) {
        std::cerr << "Не удалось создать файл: " << GetLastError() << std::endl;
        return 1;
    }


    hMapFile = CreateFileMapping(hFile, NULL, PAGE_READWRITE, 0, BUFFER_SIZE, "SharedMemory");
    if (hMapFile == NULL) {
        std::cerr << "Не удалось создать отображаемый файл: " << GetLastError() << std::endl;
        CloseHandle(hFile);
        return 1;
    }

  
    pBuf = (char*)MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, BUFFER_SIZE);
    if (pBuf == NULL) {
        std::cerr << "Не удалось отобразить файл: " << GetLastError() << std::endl;
        CloseHandle(hMapFile);
        CloseHandle(hFile);
        return 1;
    }

    ZeroMemory(&si1, sizeof(si1));
    si1.cb = sizeof(si1);
    ZeroMemory(&si2, sizeof(si2));
    si2.cb = sizeof(si2);

    std::cout << "Введите строку: ";
    std::getline(std::cin, input);

    memcpy(pBuf, input.c_str(), input.size() + 1);

    if (!CreateProcess(TEXT("child1_lab3.exe"), NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si1, &pi1)) {
        std::cerr << "Не удалось запустить процесс child1" << std::endl;
        UnmapViewOfFile(pBuf);
        CloseHandle(hMapFile);
        CloseHandle(hFile);
        return 1;
    }
    if (!CreateProcess(TEXT("child2_lab3.exe"), NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si2, &pi2)) {
        std::cerr << "Не удалось запустить процесс child2" << std::endl;
        UnmapViewOfFile(pBuf);
        CloseHandle(hMapFile);
        CloseHandle(hFile);
        return 1;
    }

    HANDLE* hChild1Process = reinterpret_cast<HANDLE*>(pBuf + BUFFER_SIZE - sizeof(HANDLE));
    *hChild1Process = pi1.hProcess;

    WaitForSingleObject(pi2.hProcess, INFINITE);

    std::cout << "Результат: " << pBuf << std::endl;


    UnmapViewOfFile(pBuf);
    CloseHandle(hMapFile);
    CloseHandle(hFile);
    CloseHandle(pi1.hProcess);
    CloseHandle(pi1.hThread);
    CloseHandle(pi2.hProcess);
    CloseHandle(pi2.hThread);

    return 0;
}
