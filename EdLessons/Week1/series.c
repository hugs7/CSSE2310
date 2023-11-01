#include <stdio.h>

/* Prints the numbers 1 through to 10 on separate lines */
void simple_number_printer(void) {
    for (int i = 1; i <= 10; i++) {
        printf("%d\n", i);
    }
}

int main(int argc, char** argv){
    simple_number_printer();
}
