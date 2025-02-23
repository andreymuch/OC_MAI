#include <iostream>
#include <string>
#include <windows.h>

typedef double (*DerivativeFunc)(double, double);
typedef double (*EFunc)(double);

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cout << "Please provide a number as an argument." << std::endl;
        return 1;
    }

    HMODULE hModule = LoadLibrary("lab4_dynamic.dll");
    if (!hModule) {
        std::cout << "Failed to load the DLL." << std::endl;
        return 1;
    }

    if (std::string(argv[1]) == "1") {
        if (argc == 4) {
            DerivativeFunc Derivative = (DerivativeFunc)GetProcAddress(hModule, "Derivative");
            if (!Derivative) {
                std::cout << "Failed to get the function address." << std::endl;
                FreeLibrary(hModule);
                return 1;
            }
            std::cout << Derivative(std::stod(argv[2]), std::stod(argv[3])) << std::endl;
            FreeLibrary(hModule);
            return 0;
        } else {
            std::cout << "неверное количетво парамтров" << std::endl;
            FreeLibrary(hModule);
            return 1;
        }
    } else if (std::string(argv[1]) == "2") {
        if (argc == 3) {
            EFunc E = (EFunc)GetProcAddress(hModule, "E");
            if (!E) {
                std::cout << "Failed to get the function address." << std::endl;
                FreeLibrary(hModule);
                return 1;
            }
            std::cout << E(std::stod(argv[2])) << std::endl;
            FreeLibrary(hModule);
            return 0;
        } else {
            std::cout << "неверное количетво парамтров" << std::endl;
            FreeLibrary(hModule);
            return 1;
        }
    } else {
        std::cout << "Invalid argument. Please provide '1' or '2'." << std::endl;
        FreeLibrary(hModule);
        return 1;
    }
}
