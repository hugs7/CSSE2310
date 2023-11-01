// CSSE2310 Assignment 3 a3
// By Hugo Burton
// Start Date: 4pm Friday 15th April, 2022
// Due Date: 4pm Friday 13th May, 2022
// Student Number: s4698512 (46985123)

#ifndef hq_h
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <csse2310a3.h>     // given library
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>

// Defintions

#define MAX_BUFFER_SIZE 52

#endif      // hq_h

// Globals

int jobid = 0;

// Program

int main(int argc, char* argv[]) {
    char* input;
    char** tokens;

    signal(SIGINT, handle_signal);
    printf("> ");
    fflush(stdout);
    // prompt loop
    while ((input = read_line(stdin)) != NULL) {
        int down[2];    // parent write, child read
        int up[2];      // parent read, child write

        int spcs = 0;
        tokens = split_space_not_quote(input, &spcs);
        
        //char* args[spcs];
        //for (int i = 1; i < spcs; i++) {
            //args[i - 1] = tokens[i];
        //}
        args[spcs] = 0;
        printf("%s, %s", args[0], args[1]);
        fflush(stdout);
        if (!strcmp(tokens[0], "spawn")) {
            // spawn (yet to put into function)
                    
            printf("New Job ID [%d] created.\n", jobid++);
            
            if (pipe(down) < 0) {
                perror("Creating down pipe");
                exit(1);
            }

            if (pipe(up) < 0) {
                perror("Creating up pipe");
                exit(1);
            }
            printf("Started (pid %i)\n", getpid());
            if (!fork()) {
                // child
                printf("child (pid %i)\n", getpid());
                fflush(stdout);
                close(down[1]);
                close(up[0]);
                
                dup2(down[0], STDIN_FILENO);
                dup2(STDOUT_FILENO, up[1]);
                execvp(args[0], args);
                perror(strcat("Trying to run ", tokens[1]));
                exit(99);
                close(down[0]);
                close(up[1]);
            } else {
                // parent
                close(down[0]);
                close(up[1]);

                close(down[1]);
                close(up[0]);
            }

            // if exec() fails, child process exists with status 99
            //printf("%s, %s\n", args[0], args[1]);
            //spawn(tokens[1], args);
        } else if (!strcmp(tokens[0], "report")) {
            // Report
            
            int status;
            pid_t pid = wait(&status);
            printf("Process %d gave back value %d\n", pid, status);
            
        }
        printf("> ");
        fflush(stdout);
    }

    return 0;
}


// Commands to be read
//
// Spawn
// Report
// Signal
// Sleep
// Send
// rcv
// eof
// cleanup

void handle_signal(int s) {
    signal(SIGINT, handle_signal);
    fflush(stdout);
}
