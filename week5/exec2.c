#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

int main(int argc, char** argv) {
    printf("Starting\n");
    if (!fork()) {	// create a child process to replace
	char* args[2];
	args[0] = "ls";
	args[1] = 0;	// note: null ptr not string terminator
        execvp("ls", args);
        perror("Trying to run ls");
	exit(2);
    }
    wait(0);	// if we don't care about status
    printf("Wait complete\n");
    return 0;
}
