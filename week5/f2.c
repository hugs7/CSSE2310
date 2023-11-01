#include <stdio.h>
#include <unistd.h>

int main(int argc, char** argv) {
    printf("1\n");
    fork();
    printf("2\n");
    fork();
    printf("3\n");
    return 0;
}
