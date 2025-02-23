#include <windows.h>
#include <iostream>
#include <string>

#define BUFFER_SIZE 256
#define MEMORY_NAME "game_control"
#define MAX_CLIENTS 1001

struct Request {
    int clientId;
    char command[BUFFER_SIZE];
};
struct  ClientActive{
    bool isUsed;
    char clientResponse[BUFFER_SIZE];
 };

struct SharedMemory {
    Request requestQueue[BUFFER_SIZE];
    int queueHead;
    int queueTail;
    int queueSize;
    ClientActive clientResponses[MAX_CLIENTS];
    
};

HANDLE hMapFile;
SharedMemory* shm;
HANDLE requestMutex, responseMutex;
int clientId;

// Добавление запроса в кольцевую очередь
void sendRequest(const std::string& command) {
    WaitForSingleObject(requestMutex, INFINITE);

    if (shm->queueSize < MAX_CLIENTS) {
        shm->requestQueue[shm->queueTail].clientId = clientId;
        strcpy_s(shm->requestQueue[shm->queueTail].command, command.c_str());
        shm->queueTail = (shm->queueTail + 1) % MAX_CLIENTS;
        shm->queueSize++;
    }

    ReleaseMutex(requestMutex);
}

std::string getResponse() {
    std::string response;
    WaitForSingleObject(responseMutex, INFINITE);
    response = shm->clientResponses[clientId].clientResponse;
    memset(shm->clientResponses[clientId].clientResponse, 0, BUFFER_SIZE);
    ReleaseMutex(responseMutex);
    return response;
}

int main() {
    hMapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, MEMORY_NAME);
    if (!hMapFile) {
        std::cerr << "Failed to open shared memory!" << std::endl;
        return 1;
    }

    shm = (SharedMemory*)MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(SharedMemory));
    if (!shm) {
        std::cerr << "Failed to map shared memory!" << std::endl;
        CloseHandle(hMapFile);
        return 1;
    }

    requestMutex = OpenMutex(SYNCHRONIZE, FALSE, "Global\\RequestQueueMutex");
    responseMutex = OpenMutex(SYNCHRONIZE, FALSE, "Global\\ResponseMutex");

    for (int i = 0;i < MAX_CLIENTS; i++){
        if (!shm->clientResponses[i].isUsed) {
            clientId = i;
            shm->clientResponses[i].isUsed = true;
            break;
        }
        else {
            std::cerr << "No available client slots!" << std::endl;
            return 1;
        }
    }
    std::cout << "Ваш id: " << clientId << std::endl;
    std::string command;
    while (true) {
        std::cout << "Enter command: ";
        std::getline(std::cin, command);

        sendRequest(command);
        Sleep(500);
        std::cout << "Server response: " << getResponse() << std::endl;
    }

    UnmapViewOfFile(shm);
    CloseHandle(hMapFile);
    CloseHandle(requestMutex);
    CloseHandle(responseMutex);

    return 0;
}
