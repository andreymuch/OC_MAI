
#include "func1.h"
#include <math.h>



float Derivative(float A, float deltaX){
    return ( cos(A + deltaX)-cos(A) )/deltaX;  
}
float E(int x){
    return (1 + 1/x) ^ x;
}
