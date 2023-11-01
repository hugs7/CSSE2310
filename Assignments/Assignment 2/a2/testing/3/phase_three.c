#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "secret.h"

int phase_three_num(void);


void phase_three() {
    int i = 0;
    int iterations = 6298;
    int num;
    
    printf("%d\n", phase_three_num());
    
    for (i = 0; i < iterations; i++) {
        printf("%d\n", phase_three_num());
    }
}

int main(int argc, char** argv) {
    phase_three();
    return 0;
}
