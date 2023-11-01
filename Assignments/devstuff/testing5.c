#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>

#define MAX_BUFFER_SIZE 52

int main(int argc, char* argv[]) {
	
	printf("%d\n", !isdigit("3abc"));
	return 0;
}
