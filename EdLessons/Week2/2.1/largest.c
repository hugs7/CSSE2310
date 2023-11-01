#include <stdio.h>

/**
 *Returns the largest of the two integers provided.
**/
int get_largest(int arg1, int arg2) {
    return (arg1 > arg2) ? arg1 : arg2;
}

int main(int argc, char** argv) {
    printf("%d\n", get_largest(1, 3));
    printf("%d\n", get_largest(50, 1));
    printf("%d\n", get_largest(5, 5));
    return 0;
}    
