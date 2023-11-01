#include <stdio.h>
#include <unistd.h>

void X(void) {
    int p=5;
    if (fork()) {
        ++p;
    } else {
        --p;
    }
    printf("%d\n", p);
}

int main(int argc, char** argv) {
    X();
    printf("Done\n");
    return 0;
}
