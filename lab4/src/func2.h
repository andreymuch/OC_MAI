#pragma once

#ifdef BUILD_DLL
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT __declspec(dllimport)
#endif




extern "C"{
DLL_EXPORT float Derivative(float A, float deltaX);
DLL_EXPORT  float E(int x);
}