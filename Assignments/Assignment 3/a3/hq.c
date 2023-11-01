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
#include <ctype.h>
#include <stdbool.h>
#include <errno.h>

// Defintions

#define MAX_BUFFER_SIZE 52
#define READ 0
#define WRITE 1

#endif      // hq_h

// Function declarations

void handle_input(char* input, char** tokens);

void spawn(char** args, int spcs);

void report(char** args, int spcs);

void call_signal(char** args, int spcs);

void hq_wait(char** args, int spcs);

void send(char** args, int spcs);

void rcv(char** args, int spcs);

void eof(char** args, int spcs);

void cleanup(void);

void handle_signal(int s);

bool is_number(char number[]);

bool is_job(char job[]);

// Structs

struct Job {
    int pid;
    char name[MAX_BUFFER_SIZE];
    int down[2];
    int up[2];
    int status;
    int finished;
};

// Globals

/* jobid keeps track of the number of jobs (child processes) spawned. */
int jobid = -1;

/* jobs is an array of Job structs, each containing information about each
 * spawned job. */
struct Job* jobs;

int sleeping = 0;

// Program

int main(int argc, char* argv[]) {
    char* input;
    char** tokens = malloc(0);

    jobs = malloc(0 * sizeof(struct Job));
    
    signal(SIGINT, handle_signal);
    signal(SIGCHLD, handle_signal);
    printf("> ");
    fflush(stdout);
    // prompt loop
    while ((input = read_line(stdin)) != NULL) {
        signal(SIGCHLD, handle_signal);
        if (!strcmp(input, "")) {
            printf("> ");
            fflush(stdout);
            continue;
        }

        handle_input(input, tokens);
        
        printf("> ");
        fflush(stdout); 
    }
    free(input);
    free(tokens);
    free(jobs);
    return 0;
}

/* handle_input()
 * --------------
 * Handles the input given from the user by tokens
 *
 * input: array which is to buffer user input
 * tokens: array of arrays which splits user input delimited by " "
 */
void handle_input(char* input, char** tokens) {
    int spcs;
    tokens = split_space_not_quote(input, &spcs);

    char* args[spcs];
    for (int i = 1; i < spcs; i++) {
        args[i - 1] = tokens[i];
    }
    args[spcs - 1] = 0;
    fflush(stdout);

    if (!strcmp(tokens[0], "spawn")) {
        // spawn
        spawn(args, spcs);
    } else if (!strcmp(tokens[0], "report")) {
        // Report
        report(args, spcs);
    } else if (!strcmp(tokens[0], "signal")) {
        call_signal(args, spcs);
    } else if (!strcmp(tokens[0], "sleep")) {
        hq_wait(args, spcs);
    } else if (!strcmp(tokens[0], "send")) {
        send(args, spcs);
    } else if (!strcmp(tokens[0], "rcv")) {
        rcv(args, spcs);
    } else if (!strcmp(tokens[0], "eof")) {
        eof(args, spcs);
    } else if (!strcmp(tokens[0], "cleanup")) {
        cleanup();
    } else {
        printf("Error: Invalid command\n");
    }

    free(input);
    free(tokens);

}

/* spawn()
 * -------
 * Runs specified program in a new process with optional arguments provided.
 * Connects stdin and stdout of new process into pipes hq can access. Stderr 
 * of the child process is unchanged
 *
 * args: arguments provided in command line without first command
 * spcs: number of original tokens
 */
void spawn(char** args, int spcs) {
    if (spcs < 2) {
        printf("Error: Insufficient arguments\n");
    } else {
        jobid++;

        jobs = realloc(jobs, (jobid + 1) * sizeof(struct Job));
        
        strcpy(jobs[jobid].name, args[0]);
        
        if (pipe(jobs[jobid].down) < 0) {
            perror("Creating down pipe");
            exit(1);
        }

        if (pipe(jobs[jobid].up) < 0) {
            perror("Creating up pipe");
            exit(1);
        }

        printf("New Job ID [%d] created\n", jobid);
        fflush(stdout);

        jobs[jobid].pid = fork();
        if (!jobs[jobid].pid) {
            // child

            close(jobs[jobid].down[WRITE]);
            close(jobs[jobid].up[READ]);
            
            dup2(jobs[jobid].down[READ], STDIN_FILENO);
            close(jobs[jobid].down[READ]);
            dup2(jobs[jobid].up[WRITE], STDOUT_FILENO);
            close(jobs[jobid].up[WRITE]);
            
            int status;
            status = execvp(args[0], args);
            if (status == -1) {
                exit(99);
            }
        } else {
            // parent
            close(jobs[jobid].down[READ]);
            close(jobs[jobid].up[WRITE]);
            
            jobs[jobid].finished = waitpid(jobs[jobid].pid, 
                    &jobs[jobid].status, WNOHANG);
        }
    }
}

/* report()
 * --------
 * Produces an outline to stdout of current child processes and their statuses
 *
 * args: arguments provided in command line without first command
 * spcs: number of original tokens
 */
void report(char** args, int spcs) {
    fflush(stdout);
    // Job status output dependent on arg present status

    // check args
    if (spcs == 1) {    // report all jobs
        printf("[Job] cmd:status\n");
        for (int i = 0; i <= jobid; i++) {
            jobs[i].finished = waitpid(jobs[i].pid, &jobs[i].status, WNOHANG);

            printf("[%d] %s:", i, jobs[i].name);
            if (!jobs[i].finished) {
                printf("running\n");
            } else if (WIFEXITED(jobs[i].status)) {
                printf("exited(%d)\n",WEXITSTATUS(jobs[i].status));
            } else if (WIFSIGNALED(jobs[i].status)) {
                printf("signalled(%d)\n", WTERMSIG(jobs[i].status));
            } else {
                printf("Error\n");
            }
        }

    } else if (spcs > 1) {      // report on specific job
        // check for job tokens[1]
        if (is_job(args[0])) { 
            int jobq = atoi(args[0]);
            jobs[jobq].finished = waitpid(jobs[jobq].pid, &jobs[jobq].status,
                    WNOHANG);

            printf("[Job] cmd:status\n");
            printf("[%d] %s:", jobq, jobs[jobq].name);

            if (!jobs[jobq].finished) {
                printf("running\n");
            } else if (WIFEXITED(jobs[jobq].status)) {
                printf("exited(%d)\n",WEXITSTATUS(jobs[jobq].status));
            } else if (WIFSIGNALED(jobs[jobq].status)) {
                printf("signalled(%d)\n", WTERMSIG(jobs[jobq].status));
            } else {
                printf("Error\n");
            }
        } else {
            printf("Error: Invalid job\n");
        }
    }
}

/* call_signal()
 * -------------
 * Sends signal to child process specified
 *
 * args: arguments provided in command line without first command
 * spcs: number of original tokens
 */
void call_signal(char** args, int spcs) {
    if (spcs < 3) {
        printf("Error: Insufficient arguments\n");
    } else {
        if (is_job(args[0])) {
            int job = atoi(args[0]);
            if (is_number(args[1])){
                int sgnl = atoi(args[1]);
                if (sgnl >= 1 && sgnl <= 31) {
                    int pid = jobs[job].pid;
                    kill(pid, sgnl);
                } else {
                    printf("Error: Invalid signal\n");
                }
            } else {
                printf("Error: Invalid signal\n");
            }
        } else {
            printf("Error: Invalid job\n");
        }
    }
}

/* hq_wait()
 * ---------
 * Causes hq to wait for specified seconds
 *
 * args: arguments provided in command line without first command
 * spcs: number of original tokens
 */
void hq_wait(char** args, int spcs) {
    if (spcs < 2) {
        printf("Error: Insufficient arguments\n");
    } else {
        // loop over input
        int digit = 1;
        if (!strcmp(args[0], "")) {
            digit = 0;
        }
        for (int i = 0; args[0][i] != 0; i++) {
            if (!isdigit(args[0][i]) && !(args[0][i] == '.')) {
                digit = 0;
            }
        }
        if (digit) {
            char* ptr;
            double slptime = strtod(args[0], &ptr);
            usleep(slptime * 1000000);
            signal(SIGCHLD, SIG_IGN);
        } else {
            printf("Error: Invalid sleep time\n");
        }
    }
}

/* send()
 * ------
 * Sends text down the pipe to specified job
 *
 * args: arguments provided in command line without first command
 * spcs: number of original tokens
 */
void send(char** args, int spcs) {
    if (spcs < 3) {
        printf("Error: Insufficient arguments\n");
    } else {
        if (is_job(args[0])) {
            int job = atoi(args[0]);
            FILE* writef = fdopen(jobs[job].down[WRITE], "w");
            fprintf(writef, "%s\n", args[1]);
            fflush(writef);
        } else {
            printf("Error: Invalid job\n");
        }
    }
}

/* rcv()
 * -----
 * Receives data through the pipe from the specified child process.
 * Prints this to stdout in the parent, hq.
 *
 * args: arguments provided in command line without first command
 * spcs: number of original tokens
 */
void rcv(char** args, int spcs) {
    if (spcs < 2) {
        printf("Error: Insufficient arguments\n");
    } else {
        if (is_job(args[0])) {
            int job = atoi(args[0]);
            int ready = is_ready(jobs[job].up[READ]);
            FILE* readf = fdopen(jobs[job].up[READ], "r");
            if (ready) {
                char* buffer = read_line(readf);
                if (buffer == NULL) {
                    printf("<EOF>\n");
                } else {
                    printf("%s\n", buffer);
                }
            } else {
                printf("<no input>\n");
            }
        } else {
            printf("Error: Invalid job\n");
        }


        fflush(stdout);
    }
}

/* eof()
 * -----
 * Closes the stdin side of the pipe so the child process can no longer read 
 * as the pipe will be EOF.
 *
 * args: arguments provided in command line without first command
 * spcs: number of original tokens
 */
void eof(char** args, int spcs) { 
    if (spcs < 2) {
        printf("Error: Insufficient arguments\n");
    } else {
        if (is_job(args[0])) {
            int job = atoi(args[0]);
            close(jobs[job].down[WRITE]);
        } else {
            printf("Error: Invalid job\n");
        }
    }
}

/* cleanup()
 * ---------
 * Terminates all child processes spawned by hq. It does this by sending 
 * signal 9 (SIGKILL). Reaps all processes.
 *
 * args: arguments provided in command line without first command
 * spcs: number of original tokens
 */
void cleanup(void) {
    for (int job = 0; job <= jobid; job++) {
        kill(jobs[job].pid, SIGKILL);
        jobs[job].finished = waitpid(jobs[job].pid, &jobs[job].status, 0);
    }

}

/* handle_signal()
 * ---------------
 * Signal handler
 *
 * s: signal number
 */
void handle_signal(int s) {
    for (int i = 0; i <= jobid; i++) {
        jobs[i].finished = waitpid(jobs[i].pid, &jobs[i].status, WNOHANG);
    }

    signal(s, handle_signal);

    fflush(stdout); 
}

/* is_number()
 * -----------
 * Determines if inputer number[] is a number or not
 *
 * number[]: array of characters to be checked
 * Returns: true if number, false if not
 */
bool is_number(char number[]) {
    for (int i = 0; number[i] != 0; i++) {
        if (!isdigit(number[i])) {
            return false;
        }
    }
    return true;
}

/* is_job()
 * --------
 * Checks if job[] is a valid job number.
 *
 * job[]: array of characters of job number
 * Returns: true if valid job, false if not
 */
bool is_job(char job[]) {
    if (strcmp(job, "") && is_number(job)) {
        int jobNum = atoi(job);
        if (jobNum <= jobid) {
            return true;
        } else {
            return false;
        }
    } else {
        return false;
    }
}
