#include <windows.h>
#include <iostream>
#include <string>
#include <locale>

#define BUFFER_SIZE 1024

int main() {

    HANDLE hPipe1, hPipe2; // Дескрипторы для именованных каналов (pipe)
    PROCESS_INFORMATION pi1, pi2; // Структуры для хранения информации о процессе (дескриптор и идентификаторы)
    STARTUPINFO si1, si2; // Структуры, содержащие информацию о запуске процесса, такие как окна, дескрипторы и т.д.
    char buffer[BUFFER_SIZE];
    DWORD dwWritten, dwRead;
    std::string input;

    // Создаем именованный канал pipe1 для передачи данных в процессе
    hPipe1 = CreateNamedPipe("\\\\.\\pipe\\Pipe1", // Имя канала
                             PIPE_ACCESS_OUTBOUND, // Канал для записи данных
                             PIPE_TYPE_BYTE | PIPE_WAIT, // PIPE_TYPE_BYTE: побайтовая передача данных. PIPE_WAIT: режим ожидания, пока данные не будут записаны
                             PIPE_UNLIMITED_INSTANCES, BUFFER_SIZE, BUFFER_SIZE, 0, NULL); // BUFFER_SIZE: размер буфера для чтения и записи. PIPE_UNLIMITED_INSTANCES: неограниченное количество подключений

    if (hPipe1 == INVALID_HANDLE_VALUE) {
        std::cerr << "Не удалось создать pipe1" << std::endl;
        return 1;
    }

    // Создаем именованный канал pipe2 для получения данных
    hPipe2 = CreateNamedPipe(TEXT("\\\\.\\pipe\\Pipe2"), 
                             PIPE_ACCESS_INBOUND, // Канал для чтения данных
                             PIPE_TYPE_BYTE | PIPE_WAIT, 
                             PIPE_UNLIMITED_INSTANCES, BUFFER_SIZE, BUFFER_SIZE, 0, NULL);

    if (hPipe2 == INVALID_HANDLE_VALUE) {
        std::cerr << "Не удалось создать pipe2" << std::endl;
        return 1;
    }

   

    // Инициализируем структуры для запуска процессов
    ZeroMemory(&si1, sizeof(si1)); // Обнуляем структуру (инициализируем нулями)
    si1.cb = sizeof(si1); // Устанавливаем размер структуры STARTUPINFO
    ZeroMemory(&si2, sizeof(si2));
    si2.cb = sizeof(si2);

   
    // Запускаем дочерний процесс 2
    if (!CreateProcessA(TEXT("F:\\Dokument\\program\\CPP\\OC\\build\\lab1\\Debug\\child2.exe"), NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si2, &pi2)) {
        std::cerr << "Не удалось запустить процесс child2" << std::endl;
        return 1;
    }


     // Запускаем дочерний процесс 1
    if (!CreateProcess(TEXT("F:\\Dokument\\program\\CPP\\OC\\build\\lab1\\Debug\\child1.exe"), NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si1, &pi1)) { 
        std::cerr << "Не удалось запустить процесс child1" << std::endl;
        return 1;
    }

    std::cout << "Введите строку: ";
    std::getline(std::cin, input);
    std::cout << "Отправляем: " << input << std::endl;
    
    // Обнуляем буфер перед записью данных
    memset(buffer, 0, sizeof(buffer));

    // Записываем данные в pipe1 (в канал)
    /*
        hPipe1: дескриптор канала, в который записываем данные.
        input.c_str(): строка данных для отправки. Метод c_str() преобразует std::string в C-строку (массив символов).
        input.size() + 1: размер данных для записи, включая нулевой символ в конце строки.
        &dwWritten: указатель на переменную, в которую будет записано количество отправленных байтов.
        NULL: параметр OVERLAPPED для асинхронных операций, NULL означает синхронный режим.
    */
    WriteFile(hPipe1, input.c_str(), input.size() + 1, &dwWritten, NULL);
    CloseHandle(hPipe1);

    // Ожидаем завершения второго дочернего процесса
    /*
        pi2.hProcess: дескриптор процесса, который был создан с помощью CreateProcess.
        INFINITE: означает, что функция WaitForSingleObject будет ждать бесконечно, пока процесс не завершится.
    */
    WaitForSingleObject(pi2.hProcess, INFINITE);

    /* 
        Чтение данных из pipe2:
        hPipe2: дескриптор канала, из которого мы будем читать данные.
        buffer: массив для хранения прочитанных данных.
        BUFFER_SIZE: максимальное количество байтов для чтения.
        &dwRead: указатель на переменную, в которую будет записано количество прочитанных байтов.
        NULL: параметр OVERLAPPED для асинхронных операций, NULL означает синхронный режим.
    */
    // Читаем данные из pipe2 после завершения дочернего процесса
    ReadFile(hPipe2, buffer, BUFFER_SIZE, &dwRead, NULL);
    CloseHandle(hPipe2);

    // Выводим результат
    std::cout << "Результат: " << buffer << std::endl;

    // Закрываем дескрипторы дочерних процессов
    // После завершения процесса необходимо закрыть его дескрипторы: pi1.hProcess и pi1.hThread
    CloseHandle(pi1.hProcess);
    CloseHandle(pi1.hThread);
    CloseHandle(pi2.hProcess);
    CloseHandle(pi2.hThread);
    
    return 0;
}
