
#include "func2.h"
#include <math.h>



float Derivative(float A, float deltaX){
    return (cos(A + deltaX)-cos(A-deltaX))/(2*deltaX);  
}
float E(int x){
    double result = 0;
    double n = 1;
    for (int i = 1; i <= x; i++){
        n *= i;
        result += 1/(n);
    }
    return result;
}
