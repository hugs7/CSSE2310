#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char** argv) {
    printf("Started (pid %i)\n", getpid());
    fflush(stdout);
    if (!fork()) {
        printf("child (pid %i)\n", getpid());
        fflush(stdout);
        sleep(10);
        printf("child exiting(17)\n");
        fflush(stdout);
        exit(17);
    }
    int status;
    pid_t pid=wait(&status);
    if (pid < 0) {
        fprintf(stderr, "Couldn't wait for some reason\n");
	exit(1);
    }
    printf("Process %d gave back value %d\n", pid, status);
    exit(0);
}
