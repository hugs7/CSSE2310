#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

void do_work(void) {
    for (int i=0; i<5; ++i) {
        if (!fork()) {
	    sleep(1);           // Simulate doing something useful
            printf("Child\n");
            exit(0);	        // not a wonderful idea
	}
        printf("Parent\n");
    }
}

int main(int argc, char** argv) {
    do_work();
    return 0;
}
