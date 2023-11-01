#include <stdio.h>
#include <unistd.h>

int main(int argc, char** argv) {
    if (argc<3) {
        fprintf(stderr, "Not enough args\n");
	return 1;
    }
    printf("Pre-exec\n");
    if (execl(argv[1], argv[1], argv[2], NULL)==-1) {
        printf("Post-exec\n");
	perror("Running: ");
    }
    return 2;
}
