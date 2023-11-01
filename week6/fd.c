#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

int main(int argc, char** argv) {
    int fd = open("fd.c", O_RDONLY);
    if (fd < 0) {
        perror("Opening");
        return 1;
    }
    char buff;
    while (read(fd, &buff, 1)==1) { // not efficient
        printf("%c", buff);
    }
    close(fd);
    return 0;
}
