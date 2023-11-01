#include <stdio.h>

/*
** Prints an extended format of the sum of arg1 and arg2 provided.
*/
void print_sum_better(int arg1, int arg2) {
    // Check if either number is negative
    if (arg1 < 0 || arg2 < 0) {
        fprintf(stderr, "One or more of the numbers entered are not positive\n");
    } else {
        printf("%d + %d = %d\n", arg1, arg2, (arg1 + arg2));
    }
}

int main(void) {
    print_sum_better(10, 20);
    print_sum_better(0, 0);
    print_sum_better(-1, -10);
    print_sum_better(-1, 10);
    print_sum_better(4096, -4096);
    
    return 0;
}
