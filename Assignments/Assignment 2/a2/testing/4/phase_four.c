#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int add(int* a, int* b);

void phase_four() {
    int j = 6327968;
    int numbers_added = 0;
    int a = 55, b = 89;
    int ptr = 0;
    while (numbers_added != 6) {
        j = add(&a, &b);

        //printf("%d, %d\n", a, b);
        ptr++;
        if (j % 7 == 0 || j % 2 == 0) {
            printf("%d\n", j);
            numbers_added++;
        }
    }
}

int add(int* a, int* b) {
    int c = *a + *b;
    *a = *b;
    *b = c;
    return c;
}

int main(int argc, char** argv) {
    phase_four();
    return 0;
}
