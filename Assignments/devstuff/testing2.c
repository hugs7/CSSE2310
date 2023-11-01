#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>

#define MAX_BUFFER_SIZE 52

int main(int argc, char* argv[]) {
	int* string = malloc(MAX_BUFFER_SIZE * sizeof(int));

	for (int i = 0; i < 50; i++) {
		string[i] = i;
	}

	for (int i = 0; i < 50; i++) {
		printf("%d\n", string[i]);
	}
	free(string);

	return 0;
}
