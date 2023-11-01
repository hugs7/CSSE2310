#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>


int main(int argc, char** argv) {
    int fd=open("listing", O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);
    printf("%i\n",fd);
    dup2(fd, 1);
    close(fd);
    execlp("ls", "ls", "-l", NULL);
    return 0;
}
