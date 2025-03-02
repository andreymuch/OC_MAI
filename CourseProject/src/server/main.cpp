#include <windows.h>
#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>

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


struct Game {
    std::string secretNumber;
    int maxPlayers;
    int currentPlayers;
    std::vector<int> players;
};
std::map<std::string, Game> activeGames;

HANDLE hMapFile;
SharedMemory* shm;
HANDLE requestMutex, responseMutex;

std::string generateSecretNumber() {
    std::string num = "";
    bool used[10] = {false};
    while (num.length() < 4) {
        int digit = rand() % 10;
        if (!used[digit]) {
            num += std::to_string(digit);
            used[digit] = true;
        }
    }
    return num;
}

std::pair<int, int> countBullsAndCows(const std::string &secret, const std::string &guess) {
    int bulls = 0, cows = 0;
    for (int i = 0; i < 4; ++i) {
        if (guess[i] == secret[i]) bulls++;
        else if (secret.find(guess[i]) != std::string::npos) cows++;
    }
    return {bulls, cows};
}

bool dequeueRequest(Request &req) {
    WaitForSingleObject(requestMutex, INFINITE);
    if (shm->queueSize == 0) {
        ReleaseMutex(requestMutex);
        return false;
    }

    req = shm->requestQueue[shm->queueHead];
    shm->queueHead = (shm->queueHead + 1) % MAX_CLIENTS;
    shm->queueSize--;

    ReleaseMutex(requestMutex);
    return true;
}

void handleClientRequests() {
    while (true) {
        Request req;
        if (dequeueRequest(req)) {
            std::istringstream iss(req.command);
            std::string command, gameName, param;
            iss >> command >> gameName >> param;

            std::string response;

            if (command == "create") {
                if (param.empty() || !std::isdigit(param[0])) {
                    response = "Error: Specify the number of players!";
                } else {
                    int maxPlayers = std::stoi(param);
                    if (maxPlayers < 1 || maxPlayers > 100) {
                        response = "Error: Players count must be 1-100.";
                    } else if (activeGames.count(gameName)) {
                        response = "Error: Game already exists!";
                    } else {
                        activeGames[gameName] = {generateSecretNumber(), maxPlayers, 0, {}};
                        response = "Game " + gameName + " created for " + param + " players!";
                    }
                }
            } 
            else if (command == "join" || command == "find") {
                std::map<std::string, Game>::iterator it;

                if (command == "join") {
                    it = activeGames.find(gameName);
                } else { // find
                    it = std::find_if(activeGames.begin(), activeGames.end(), [](const auto& pair) {
                        return pair.second.currentPlayers < pair.second.maxPlayers;
                    });
                    if (it == activeGames.end()) {
                        response = "Error: No available games!";
                    } else {
                        gameName = it->first;
                    }
                }

                if (it == activeGames.end()) {
                    response = "Error: Game not found!";
                } else if (it->second.currentPlayers >= it->second.maxPlayers) {
                    response = "Error: Game is full!";
                } else {
                    it->second.players.push_back(req.clientId);
                    it->second.currentPlayers++;
                    response = "Joined game " + gameName + ". Enter 'guess <number>' or 'exit'.";

                    WaitForSingleObject(responseMutex, INFINITE);
                    strcpy_s(shm->clientResponses[req.clientId].clientResponse, response.c_str());
                    ReleaseMutex(responseMutex);

                    while (true) {
                        Request gameReq;
                        if (dequeueRequest(gameReq) && gameReq.clientId == req.clientId) {
                            std::istringstream iss(gameReq.command);
                            std::string gameCmd, gameParam;
                            iss >> gameCmd >> gameParam;

                            if (gameCmd == "exit") {
                                response = "You left the game.";
                                it->second.players.erase(
                                    std::remove(it->second.players.begin(), it->second.players.end(), req.clientId),
                                    it->second.players.end()
                                );
                                it->second.currentPlayers--;
                                break;
                            }
                            else if (gameCmd == "guess") {
                                if (gameParam.length() != 4 || !std::all_of(gameParam.begin(), gameParam.end(), ::isdigit)) {
                                    response = "Error: Enter a valid 4-digit number!";
                                } else {
                                    auto [bulls, cows] = countBullsAndCows(it->second.secretNumber, gameParam);
                                    response = "Bulls: " + std::to_string(bulls) + ", Cows: " + std::to_string(cows);
                                    if (bulls == 4) {
                                        response += " 🎉 You guessed it!";
                                        activeGames.erase(it);
                                        break;
                                    }
                                }
                            } else {
                                response = "Error: Invalid command! Use 'guess <number>' or 'exit'.";
                            }

                            WaitForSingleObject(responseMutex, INFINITE);
                            strcpy_s(shm->clientResponses[req.clientId].clientResponse, response.c_str());
                            ReleaseMutex(responseMutex);
                        }
                        Sleep(100);
                    }
                }
            }
            else {
                response = "Error: Invalid command!";
            }

            WaitForSingleObject(responseMutex, INFINITE);
            strcpy_s(shm->clientResponses[req.clientId].clientResponse, response.c_str());
            ReleaseMutex(responseMutex);
        }
        Sleep(100);
    }
}

int main() {
    srand(time(0));

    hMapFile = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(SharedMemory), MEMORY_NAME);
    if (!hMapFile) {
        std::cerr << "Failed to create shared memory!" << std::endl;
        return 1;
    }

    shm = (SharedMemory*)MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(SharedMemory));
    if (!shm) {
        std::cerr << "Failed to map shared memory!" << std::endl;
        CloseHandle(hMapFile);
        return 1;
    }

    shm->queueHead = 0;
    shm->queueTail = 0;
    shm->queueSize = 0;

    requestMutex = CreateMutex(NULL, FALSE, "Global\\RequestQueueMutex");
    responseMutex = CreateMutex(NULL, FALSE, "Global\\ResponseMutex");

    std::cout << "Server started. Waiting for clients..." << std::endl;
    handleClientRequests();

    UnmapViewOfFile(shm);
    CloseHandle(hMapFile);
    CloseHandle(requestMutex);
    CloseHandle(responseMutex);

    return 0;
}
