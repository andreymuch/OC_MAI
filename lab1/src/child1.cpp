#include <windows.h>
#include <iostream>
#include <cctype>
#include <locale> 



#define BUFFER_SIZE 1024

int main() {
    HANDLE hPipe, hOut;
    char buffer[BUFFER_SIZE];
    DWORD dwRead, dwWritten;
    std::locale loc("ru_RU.UTF-8");
    
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    // Подключение к pipe1 для чтения данных от родительского процесса
    hPipe = CreateFile("\\\\.\\pipe\\Pipe1",
                       GENERIC_READ,//прова на чтение 
                       0, NULL, OPEN_EXISTING, 0, NULL);

    if (hPipe == INVALID_HANDLE_VALUE) {
        std::cerr << "Failed to connect to pipe1" << std::endl;
        return 1;
    }

    // Инициализация буфера
    memset(buffer, 0, sizeof(buffer));

    // Чтение данных из pipe1
    ReadFile(hPipe, buffer, BUFFER_SIZE, &dwRead, NULL);
    CloseHandle(hPipe);

    // Преобразование строки в верхний регистр
    for (int i = 0; buffer[i]; i++) {
        buffer[i] = toupper(buffer[i], loc);
    }
    while (true) {
        hOut = CreateFile("\\\\.\\pipe\\Pipe3",
                       GENERIC_WRITE,//прова на запись 
                       0, NULL, OPEN_EXISTING, 0, NULL);
        if (hOut != INVALID_HANDLE_VALUE) {
            std::cerr << "Успешное подключение к pipe3 в child2" << std::endl;
            break;
        }
        else{
             std::cerr << "неудачное подключение к pipe3 в child2" << std::endl;
        }
        Sleep(100);
    }
    WriteFile(hOut, buffer, dwRead, &dwWritten, NULL);
    CloseHandle(hOut);
    

    return 0;
}
