#include <stdio.h>

/* 
** Prints an extended format of the sum of arg1 and arg2 provided. 
*/ 
void print_sum(int arg1, int arg2) {
    printf("%d + %d = %d\n", arg1, arg2, arg1 + arg2);
}

int main(void) {
    print_sum(10, 20);
    print_sum(0, 0);
    print_sum(-1, -10);
    print_sum(-1, 10);
    print_sum(4096, -4096);
    
    return 0;
}
