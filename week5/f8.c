#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

#define NUM_PROCS 8

int main(int argc, char** argv) {
    pid_t ids[NUM_PROCS];
    for (int i=0; i < NUM_PROCS; ++i) {
        printf("PARENT: spawning child\n");
        fflush(stdout);
        if (!(ids[i]=fork())) {
            printf("  CHILD:  PID %d has value %d\n", getpid(), i);
            srand(getpid());        // Initialise the random number generator
            sleep(rand() % 7);      // Sleep for a random # of seconds
	    if (i%3) {
                printf("  CHILD:   (%i) gonna SEGFAULT\n",getpid());
                fflush(stdout);
                *((int*)0)=17;
	    } else {
                printf("  CHILD:  (%i) exiting normally\n", getpid());
                exit(i);
            }
	}
    }
    int status;
    sleep(1);
    for (int i=0;i < NUM_PROCS; ++i) {
        if (waitpid(ids[i], &status, 0)<0) {
            continue;
	}
	printf("PARENT: Process %d ", ids[i]);
	if (WIFEXITED(status)) {
            printf(" exited with %d\n", WEXITSTATUS(status));
	} else if (WIFSIGNALED(status)) {
            printf(" died due to signal %d\n", WTERMSIG(status));
	} else {
            printf(" doesn't know what happened\n");
	}
    }
    return 0;
}

