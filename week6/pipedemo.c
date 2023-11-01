#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <string.h>

int main(int argc, char *argv[]) {

    int fd[2];

    if(pipe(fd)<0) {
        perror("Creating pipe");
        exit(1);
    }

    if(fork()) {
        //parent
        close(fd[0]);  // Close read end in parent

        char buffer[50];
        for (int i=0; i<30; i++) {
            // We could also use dprintf(fd, ...) BTW...
            sprintf(buffer, "%i\n", i);
            write(fd[1], buffer, strlen(buffer));
            //usleep(50000);
        }
        close(fd[1]);

        // wait on our child - what happens if we don't do this?
        wait(NULL);

        exit(0);
    } else {
        // child
        close(fd[1]); // close write end in child
        char buffer[50];

        size_t got;
        while((got = read(fd[0], buffer, 50)) > 0) {
            printf("(child read %i bytes)\n", (int)got);
            for(int i=0;i < got; i++) {
                fputc(buffer[i], stdout);
            }
        }
        close(fd[0]);
        exit(0);
    }
}

