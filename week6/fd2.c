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
    char buff[20];
    size_t got;
    while ((got=read(fd, &buff, 20))>0) { // 
        for (int i = 0; i < got; ++i) {
            printf("%c", buff[i]);
        }
    }
    close(fd);
    return 0;
}
