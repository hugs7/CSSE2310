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
#include <signal.h>

// Defintions

#define MAX_BUFFER_SIZE 52

#endif      // hq_h

// Function declarations

void spawn(char* program, char** args);

void report(int jobid);

void call_signal(int jobid, int signum);

void send(int jobid, char* text);

void rcv(int jobid);

void eof(int jobid);

void cleanup(void);

void handle_signal(int s);

// Globals

int jobid = 0;

// Struct

struct Job {
    int id;
    char* name;
    int down[2];
    int up[2];
};

// Program

int main(int argc, char* argv[]) {
    char* input;
    char** tokens;
    
    int* pids = (int*)calloc(0, sizeof(int));
    char** pnames = (char**)calloc(0, sizeof(char*));
    int newPid;

    int down[2];
    int up[2];

    signal(SIGINT, handle_signal);
    printf("> ");
    fflush(stdout);
    // prompt loop
    while ((input = read_line(stdin)) != NULL) {
        int spcs = 0;

        if (!strcmp(input, "")) {
            printf("> ");
            fflush(stdout);
            continue;
        }
        tokens = split_space_not_quote(input, &spcs);
        
        char* args[spcs];
        for (int i = 1; i < spcs; i++) {
            strcpy(args[i - 1], tokens[i]);
        }
        args[spcs - 1] = 0;
        fflush(stdout);
        if (!strcmp(tokens[0], "spawn")) {
            // spawn
            
            // create new job

            if (pipe(down) < 0) {
                perror("Creating down pipe");
                exit(1);
            }

            if (pipe(up) < 0) {
                perror("Creating up pipe");
                exit(1);
            }

            printf("Piped\n");
            printf("New Job ID [%d] created.\n", jobid);

            printf("Started (pid %i)\n", getpid());

            newPid = fork();
            if (!newPid) {
                // child

                printf("child (pid %i)\n", getpid());
                fflush(stdout);
                close(down[1]);
                close(up[0]);
                
                dup2(down[0], STDIN_FILENO);
                close(down[0]);
                dup2(up[1], STDOUT_FILENO);
                close(up[1]);
                
                fflush(stdout);
                
                execvp(args[0], args);
                
                perror(strcat("Trying to run ", tokens[1]));
                exit(99);
            } else {
                // parent
                close(down[0]);
                close(up[1]);
                                
                pids = (int*)realloc(pids, (jobid + 1) * sizeof(int));
                pids[jobid] = newPid;
                pnames = (char**)realloc(pnames, (jobid + 1) * sizeof(char*));
                printf("jobid %d, name %s, pid %i\n", jobid, args[0], newPid);
                pnames[jobid] = (char*)calloc(1, MAX_BUFFER_SIZE * sizeof(char));
                strcpy(pnames[jobid], args[0]);
                jobid++;
                
                wait(NULL);

                close(down[1]);
                close(up[0]);
            }
        } else if (!strcmp(tokens[0], "report")) {
            // Report
            fflush(stdout);
            
            int status;

            // Header row
            printf("[Job] cmd:status\n");
            
            // Job status output dependent on arg present status

            // check args
            if (spcs == 1) {    // report all jobs
                //pid_t pid = wait(&status);
                //printf("Process %d gave back value %d\n", pid, status);
                
                for (int i = 0; i < jobid; i++) {
                    waitpid(pids[i], &status, 0);
                    printf("[%d] %s:", i, pnames[i]);

                    if (WIFEXITED(status)) {
                        printf("exited(%d)\n",WEXITSTATUS(status));
                    } else if (WIFSIGNALED(status)) {
                        printf("signalled(%d)\n", WIFSIGNALED(status));
                    } else {
                        printf("running\n");
                    }
                }

            } else if (spcs > 1) {      // report on specific job
                // check for job tokens[1] 
                int jobq = atoi(args[0]);

                if (jobq <= jobid) {
                    waitpid(pids[jobq], &status, 0);
                    printf("[%d] %s:", jobq, pnames[jobq]);

                    if (WIFEXITED(status)) {
                        printf("exited(%d)\n",WEXITSTATUS(status));
                    } else if (WIFSIGNALED(status)) {
                        printf("signalled(%d)\n", WIFSIGNALED(status));
                    } else {
                        printf("running\n");
                    }
                } else { // invalid job id
                    printf("Error: Invalid job\n");
                }
            }
            
        } else if (!strcmp(tokens[0], "signal")) {
            if (spcs < 3) {
                printf("Error: Insufficient arguments.\n");
            }
            
            int job = atoi(args[0]);
            int sgnl = atoi(args[1]);
            
            int pid = pids[job];

            kill(pid, sgnl);
            
        } else if (!strcmp(tokens[0], "sleep")) {
            if (spcs < 2) {
                printf("Error: Insufficient arguments.\n");
            }

            //delay(atoi(args[0]) * 1000);
        } else if (!strcmp(tokens[0], "send")) {
            if (spcs < 3) {
                printf("Error: Insufficient arguments.\n");
            }

            //char buffer[MAX_BUFFER_SIZE];
            //write(down[1], args[2]);
        } else if (!strcmp(tokens[0], "rcv")) {
            if (spcs < 2) {
                printf("Error: Insufficient arguments.\n");
            }

            char buffer[MAX_BUFFER_SIZE];
            size_t got;
            
            while ((got = read(up[0], buffer, MAX_BUFFER_SIZE)) > 0) {
                printf("Read %i bytes\n.", (int)got);
                for (int i = 0; i < got; i++) {
                    fputc(buffer[i], stdout);
                }
            }
            
            fflush(stdout);

        } else if (!strcmp(tokens[0], "eof")) {
            if (spcs < 2) {
                printf("Error: Insufficient arguments.\n");
            }

        } else if (!strcmp(tokens[0], "cleanup")) {
            //signal(SIGKILL, handle_signal);
            //kill(
            cleanup();
        } else {
            printf("Error: Invalid command\n");
        }

        printf("> ");
        fflush(stdout);
    }
    
    free(pids);
    for (int i = 0; i < jobid; i++) {
        free(pnames[i]);
    }
    free(pnames);

    return 0;
}


void handle_signal(int s) {
    signal(s, handle_signal);
    fflush(stdout);
}
