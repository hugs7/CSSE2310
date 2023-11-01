// CSSE2310 Assignment 3 a3
// By Hugo Burton
// Start Date: 4pm Friday 15th April, 2022
// Due Date: 4pm Friday 13th May, 2022
// Student Number: s4698512 (46985123)

#ifndef sigcat_h

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <csse2310a3.h>     // given library

#define MAX_BUFFER_SIZE 52

#endif      // sigcat_h

int outputStream = 0;     // global variable for signals occuring

/*
 * handle_signal()
 * ---------------
 * A handler function for all signals 1 through 31 inclusive
 * with exception to SIGKILL (9) and SIGSTOP (19) both of which
 * cannot be blocked.
 *
 * s: the signal number called
 *
 * Returns: void
 */
void handle_signal(int s) {
    if (s >= 1 && s <= 31) {
        // Valid signal
        if (!outputStream) {
            fprintf(stdout, "sigcat received %s\n", strsignal(s));
        } else {
            fprintf(stderr, "sigcat received %s\n", strsignal(s));
        }
        fflush(stdout);
        fflush(stderr);
    }
    switch (s) {
        case SIGUSR1:
            // Set output stream to stdout
            outputStream = 0;
            break;
        case SIGUSR2:
            // Set output steam to stderr
            outputStream = 1;
            break;
        default:
            break;
    }
    fflush(stdout);
    if (s != 10 && s != 12) {
        exit(0);
    }
}

/*
 * free_mem()
 * ----------
 * Frees dynamically allocated memory used in the program
 *
 * input: char pointer used to buffer user input from stdin
 *
 * Returns: void
 */
void free_mem(char* input) {
    free(input);
}

int main(int argc, char* argv[]) {
    char* input = calloc(MAX_BUFFER_SIZE, sizeof(char));
    
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));     // Keep valgrind happy
    sa.sa_handler = handle_signal;
    sa.sa_flags = SA_RESTART;
    
    for (int i = 0; i <= 31; i++) {
        if (i == 9 || i == 19) {
            // unhandlable
            continue;
        }
        sigaction(i, &sa, 0);
    }
    
    while ((input = read_line(stdin)) != NULL) {
        if (input == NULL) {
            break;
        }
        if (!outputStream) {
            fprintf(stdout, "%s\n", input);
            fflush(stdout);
        } else {
            fprintf(stderr, "%s\n", input);
            fflush(stderr);
        }
    }
    
    free_mem(input);
    
    return 0;
}
