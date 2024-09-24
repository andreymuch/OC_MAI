#include <windows.h>
#include <iostream>
#include <string>


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
    HANDLE hPipe;
    char buffer[BUFFER_SIZE];
    DWORD dwRead, dwWritten;

    memset(buffer, 0, sizeof(buffer));

    HANDLE hPipe3 = CreateNamedPipe("\\\\.\\pipe\\Pipe3", 
                                    PIPE_ACCESS_INBOUND, 
                                    PIPE_TYPE_BYTE | PIPE_WAIT, 
                                    PIPE_UNLIMITED_INSTANCES, 
                                    BUFFER_SIZE, 
                                    BUFFER_SIZE, 
                                    0, 
                                    NULL);
    if (hPipe3 == INVALID_HANDLE_VALUE) {
        std::cerr << "Не удалось создать pipe3" << std::endl;
        return 1;
    }

    // Ожидание подключения (ожидает, пока child1 подключится и начнет запись)
    BOOL connected = ConnectNamedPipe(hPipe3, NULL) ? TRUE : (GetLastError() == ERROR_PIPE_CONNECTED);

    if (!connected) {
        std::cerr << "Ошибка при подключении к pipe3" << std::endl;
        CloseHandle(hPipe3);
        return 1;
}


    ReadFile(hPipe3, buffer, BUFFER_SIZE, &dwRead, NULL);
    CloseHandle(hPipe3);

    remove_duplicate_spaces(buffer);

    // Подключение к pipe2
    hPipe = CreateFile("\\\\.\\pipe\\Pipe2",
                    GENERIC_WRITE,
                    0, NULL, OPEN_EXISTING, 0, NULL);

    if (hPipe == INVALID_HANDLE_VALUE) {
        std::cerr << "Failed to connect to pipe2" << std::endl;
        return 1;
    }

    WriteFile(hPipe, buffer, dwRead, &dwWritten, NULL);
    CloseHandle(hPipe);

    return 0;
}
