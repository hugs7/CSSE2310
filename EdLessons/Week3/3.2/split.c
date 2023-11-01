#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * Separates segments of string separated by delim into array of char*
**/
char** separate_line(char* string, char delim) {
	char* newString = (char*)calloc(strlen(string), sizeof(char*));
	strcpy(newString, string);
	// Initiaise array and dynamically allocate it memory
	char** arr = (char**)calloc(0, sizeof(char*));
	int arrCount = 0;
	// Delim
	char merge[2];
	merge[0] = delim;
	merge[1] = '\0';
	// Initialise and declare necesary Varibales to use strtok
	char* token = strtok(newString, merge);
	arr = (char**)realloc(arr, ++arrCount * sizeof(char*));
	arr[arrCount - 1] = (char*)calloc(52, sizeof(char));
	// Extract first token
	strcpy(arr[arrCount - 1], token);
	// Extract the other tokens
	while (token != NULL) {
		token = strtok(NULL, merge);
		if (token == NULL) {
			break;
		}
		// above because we're doing all this below in the same loop
		arr = (char**)realloc(arr, ++arrCount * sizeof(char*));
		arr[arrCount - 1] = (char*) calloc(52, sizeof(char));
		strcpy(arr[arrCount - 1], token);
	}
	return arr;
}

int main(void) {
	char** result = separate_line("This is a sentence to be separated", ' ');

	for (int i = 0; i < 7; i++) {
		printf("%d:    %s\n", i, result[i]);
	}
		
	return 0;
}
