#include <stdio.h>
#include <signal.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>

bool eoChild = false;

void childHandler(int s) {
    eoChild = true;
}

void spawnChild(void) {
    printf("Spawning a child\n");
    fflush(stdout);
    if (!fork()) {
        srand(getpid());
        int delay = rand() % 30;        // Simulate working up to 30 seconds
        sleep(delay);
	exit(delay);
    }
}

int main(int argc, char** argv) {
    struct sigaction sa;
    sa.sa_handler = childHandler;
    sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;  // Only interested in end of chld 
    sigaction(SIGCHLD, &sa, 0);

    int numChildren = 0;
    for (int i=0; i < 100000; ++i) {
        printf("%d\n", i);
	if (numChildren<15) {
            numChildren++;
            spawnChild();
	}
	usleep(250000);
	if (eoChild) {
	    pid_t pid;
            while (pid=waitpid(-1, 0, WNOHANG), pid > 0) {
                printf("Child ended  - PID:%d\n", pid);
                numChildren--;
            }
	    eoChild = false;
	}

    }
    return 0;
}
