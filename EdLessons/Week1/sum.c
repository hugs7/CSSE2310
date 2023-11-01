#include <stdio.h>

/*
** Returns the sum of two integers. 
*/ 
int sum(int a, int b) {
    int c = a + b;
    return (int) c;
}

int main(void) {
    printf("%d\n", sum(10, 20));
    printf("%d\n", sum(0, 0));
    printf("%d\n", sum(-1, -10));
    printf("%d\n", sum(-1, 10));
    printf("%d\n", sum(4096, -4096));
    
    return 0;
}
