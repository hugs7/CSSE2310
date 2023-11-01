#include <stdio.h>
#include <unistd.h>

int main(int argc, char** argv) {
    printf("1 ");
    fork();
    printf("2 ");
    fork();
    printf("3\n");
    return 0;
}
