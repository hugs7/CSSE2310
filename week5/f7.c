#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <stdlib.h>

void spawn_children(void) {
    const int NUM_PROCS=5;
    for (int i=0; i < NUM_PROCS; ++i) {
        printf("PARENT: spawning child\n");
        fflush(stdout);
        if (!fork()) {
            printf("  CHILD:  PID %d has value %d\n", getpid(), i);
	    if (i%3) {
                printf("  CHILD:   (%i) gonna SEGFAULT\n", i);
                fflush(stdout);
                *((int*)0)=17;      // Deliberate segfault in children 1,2,4 
	    } else {
                printf("  CHILD:  (%i) exiting normally\n", i);
                fflush(stdout);
                exit(i);
            }
	}
    }
}

int main(int argc, char** argv) {

    spawn_children();

    int status;
    pid_t pid;
    while ((pid=wait(&status)) > -1) {
	printf("PARENT: Process %d ", pid);
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

