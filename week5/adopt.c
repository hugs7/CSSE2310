#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int main(int argc, char** argv) {
    printf("PID=%d\n", getpid());
    fflush(stdout);
    pid_t pid;
    if (pid=fork()) {
	printf("I'm the parent (%i), my child is %d\n", getpid(), pid);
        fflush(stdout);
	sleep(5);
        printf("Parent is leaving\n");
        fflush(stdout);
        return 0;
    } else {
        printf("I'm the child (%i), my parent=%d\n", getpid(), getppid());
        while(1) {
            printf("I'm the child (%i), my parent=%d\n", getpid(), getppid());
            printf("Parent=%d\n", getppid());	    
            sleep(1);    
        }
    }
    return 0;
}
