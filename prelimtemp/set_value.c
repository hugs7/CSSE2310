#include <stdio.h>

/**
 * Sets the value of the int pntr to 5, also returns the pntr arg.
**/
int* set_value(int* arg) {
    *arg = 5;
    return arg;
}

int main(void) {
    int num = 10;
    
    printf("Original: %d\n", num);
    int* p = set_value(&num);
    printf("Changed: %d %d\n", *p, num);

    return 0;
}
