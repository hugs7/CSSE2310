#include <stdio.h>
#include <stdlib.h>

/*
 * Returns characters from FILE* up to newline character or EOF
*/
char* read_line(FILE* file) {
	// Initialise memory for a new char* array
	char* buff = (char*)malloc(sizeof(char*) * 0);
	int count = 0;
	char c;
	while (1) {
		// Continue looping while not end of file
		// or at the end of a line in file
		if (c = fgetc(file), c == EOF || c == '\n') {
			buff = realloc(buff, sizeof(char*) * ++count);
			buff[count - 1] = '\0';
			break;
		}
		
		// Get next character in file (as int) using fgetc
		// Add char to array (remember to terminate array
		// with null char when ready to return it)
		buff = realloc(buff, sizeof(char*) * ++count);
		buff[count - 1] = c;
	}

	return buff;
}

int main (int argc, char** argv) {
	// Check if correct number of cmd args have been provided
	if (argc != 2) {
		// Incorrect number of arguments provided
		return 1;
	}

	// Open file as a FILE* (use fopen)
	FILE* file = fopen(argv[1], "r");
	// Use some form of loop to print first 3 lines of file
	for (int i = 0; i < 3; i++) {
		printf("%s\n", read_line(file));
	}
	// Close the file (use fclose)
	fclose(file);
	
	return 0;
}
