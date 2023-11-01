#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>

bool itHappened = false;	// global variable

void notice(int s) {	// parameter for if we handle multiple signals
    itHappened = true;
}

int main(int argc, char** argv) {
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = notice;   
    sa.sa_flags = SA_RESTART;
    sigaction(SIGINT, &sa, 0);
    while (true) {
        while (!itHappened) {
            usleep(500000);
        }
        printf("It happened\n");
	itHappened = false;
    }
    return 0;
}
