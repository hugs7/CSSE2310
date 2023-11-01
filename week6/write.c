#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char** argv) {
    int fd = open("output", O_WRONLY | O_CREAT, S_IRWXU | S_IRGRP);
    if (fd < 0) {
        perror("Opening");
        return 1;
    }
    char buff[20];
    for (int i = 0; i < 20; ++i) {
        sprintf(buff, "Line %d\n", i+1);   // could write raw bytes 
        if (write(fd, buff, strlen(buff)) < 0) {// but not human readable
            perror("Writing");
        }
    }
    close(fd);
    return 0;
}
