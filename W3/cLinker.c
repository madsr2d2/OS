#include <math.h>
#include <stdio.h>

// Compile as gcc cLinker.c -lm in order to link to the math library  
int main() {
    int base = 3;
    int power = 7;
    printf("%d to the power of %d is %.2f\n", base , power, pow(base,power));
}