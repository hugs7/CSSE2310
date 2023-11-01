#include <stdio.h>

/*
** Prints a statement explaining the sign of the arg entered.
*/
void print_sign(int arg) {

    /* Add conditional checks here, the first one has already been entered for you */
    if (arg > 0) {
        printf("This number is positive\n");
    } else if (arg == 0) {
        printf("This number is zero\n");
    } else {
        printf("This number is negative\n");
    }
}

int main(void) {
    print_sign(0);
    print_sign(1);
    print_sign(-2);
    
    return 0;
}
