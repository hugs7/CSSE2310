#include <stdio.h>
#include <unistd.h>

int main(int argc, char** argv) {
    printf("1 ");
    fflush(stdout);
    fork();
    printf("2 ");
    fflush(stdout);
    fork();
    printf("3\n");
    return 0;
}
