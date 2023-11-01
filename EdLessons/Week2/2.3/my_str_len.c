#include <stdio.h>

int my_str_len(char* string) {
    int i;
    for (i = 0; string[i] != '\0'; ++i);
    return i;
}

int main(int argc, char** argv) {
    printf("%d\n", my_str_len("Four"));
    printf("%d\n", my_str_len(""));

    return 0;
}
