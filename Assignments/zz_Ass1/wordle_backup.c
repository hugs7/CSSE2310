#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
//#include <csse2310a1.h>

/*
 * Main function which handles running the program.
*/
int main(int argc, char* argv[]) {
	// Check for command line arguments
	int i;
	int len_present = 0;
	int max_present = 0;
	
	int len_min = 3, max_min = 3;
	int len_max = 9, max_max = 9;

	int max_args = 5;
	
	char* dic_path = malloc(1);
	FILE* dic_file;
	
	if (argc > 6) {
		fprintf(stderr, "Too many arguments present");
		return 1;
	}

	for (i = 1; i < argc; i++) {
		// If argument starts with a '-'
		if (argv[i][0] == '-') {
			// Check for -len
			if (!strcmp(argv[i], "-len")) {
				// Check if argument already given
				if (len_present == 1) {
					fprintf(stderr, "Argument error: %s already given.\n", 
							argv[i]);
					return 1;
				}
				
				len_present = 1;
				printf("Argument: %s\n", argv[i]);	// Debug
				// Check for subsequent length
				int len;
				// if atoi fails, it will return 0 which will result in error 1
				len = atoi(argv[i + 1]);
				if (len >= len_min && len <= len_max) {
					printf("Length value: %d\n", len);	// Debug
					// Skip next loop as we are checking here
					i++;
				} else {
					printf("Invalid %s argument value: %d. Value must be "
							"between %d and %d", argv[i], len, len_min, 
							len_max);
					/*fprintf(stderr, "invalid %s argument value: %d. Value " + 
							"must be between %d and %d.", argv[i], len, 
							len_min, len_max);*/
					return 1;
				}
			} else if (!strcmp(argv[i], "-max")) { // Check for -max
				if (max_present == 1) {
					fprintf(stderr, "Argument error: %s already given.\n", 
							argv[i]);
					return 1;
				}
				
				max_present = 1;
				printf("Argument: %s\n", argv[i]);	// Debug
				// Check for subsequent max value
				int max;
				// if atoi fails, it will return 0 which will result in error 1
				max = atoi(argv[i + 1]);
				if (max >= max_min && max <= max_max) {
					printf("Max value: %d\n", max);	// Debug
					// Skip next loop as we are checking here
					i++;
				} else {
					/*fprintf(stderr, "invalid %s argument value: %d. Value " +
							"must be between %d and %d.", argv[i], max, max_min, 
							max_max);*/
					return 1;
				}
			} else {
				// Else if an argument starts with '-' but is not -len or -max
				// then program errors.
				fprintf(stderr, "I1nvalid argument given: %s\n", argv[i]);
				return 1;
			}
				
		} else if (!strcmp(argv[i], "")) {
			// Any argument is the empty string.
			fprintf(stderr, "I2nvalid argument given: %s\n", argv[i]);
			return 1;
		} else {
			// Check for exisiting dictionary path
			if (!strcmp(dic_path, "")) {
				// Size dic_path for size of path passed as argument
				dic_path = realloc(dic_path, sizeof(argv[i]));
				strcpy(dic_path, argv[i]);
				printf("Dictionary path: %s\nargv[i]: %s\n", dic_path, argv[i]);

				// Check given dictionary path is valid
				
				dic_file = fopen(dic_path, "w");
				if (dic_file) {
					// File can be opened
					
				} else {
					// If the dictionary file could not be opened
					fprintf(stderr, "wordle: dictionary file \"%s\" cannot be "
							"opened\n", dic_path);
				}
			} else {
				fprintf(stderr, "Too many arguments given: %s not expected\n",
						argv[i]);
				return 1;
			}
		}
	}
	
	// Clean exit of program
	return 0;
}
