#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char** argv) {
    printf("Started %d\n", getpid());
    fflush(stdout);
    if (!fork()) {
        // Child
        exit(17);
    }
    // Child has almost certainly terminated but parent is still alive
    // go run 'ps -ef | grep zom1' in another shell
    sleep(30);
    exit(0);
}
