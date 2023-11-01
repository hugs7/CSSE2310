// CSSE2310 Assignment 3 a3
// By Hugo Burton
// Start Date: 4pm Friday 15th April, 2022
// Due Date: 4pm Friday 13th May, 2022
// Student Number: s4698512 (46985123)

#ifndef hq_h
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

// Defintions

#define MAX_BUFFER_SIZE 52

#endif      // hq_h

// Function declarations



void handle_signal(int s);

// Globals

int jobid = 0;

// Struct

struct Job {
    int id;
    char name[MAX_BUFFER_SIZE];
    int down[2];
    int up[2];
};

// Program

int main(int argc, char* argv[]) {
    struct Job* jobs = malloc(0 * sizeof(struct Job));
    char* name = "ls";
    // create a job
    
    jobs = realloc(jobs, (jobid + 1) * sizeof(struct Job));
    strcpy(jobs[0].name, name);
    
    jobid++;

    jobs[0].id = 0;

    printf("%d %s\n", jobs[0].id, jobs[0].name);
    
    return 0;
}


void handle_signal(int s) {
    signal(s, handle_signal);
    fflush(stdout);
}
