#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>


int main(int argc, char** argv) {
    int in = open("stdin.txt", O_RDONLY);
    int out = open("stdout.txt", O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);
    int err = open("stderr.txt", O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);
    dup2(in, STDIN_FILENO);             // or 0
    dup2(out, STDOUT_FILENO);           // or 1
    dup2(err, STDERR_FILENO);           // or 2
    close(in);
    close(out);
    close(err);

    printf("This is going to standard out\n");
    fprintf(stderr, "This is going to standard error\n");

    char buffer[80];
    fgets(buffer, 80, stdin);

    printf("I read \"%s\" from standard input\n", buffer);
    return 0;
}
