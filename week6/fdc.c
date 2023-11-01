#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>


int main(int argc, char** argv) {
    int fd=open("letters", O_RDONLY);
    char *outfile;
    if (fork()) {
        outfile = "parent.txt";
    } else {
        outfile = "child.txt";
    }
    int wfd=open(outfile, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);
    char c;
    while (read(fd, &c, 1)>0) {
        // Sleep very briefly to let the parent and child race take place
        usleep(100);
        write(wfd, &c, 1);
    }
    close(fd);
    close(wfd);
    return 0;
}
