// Wordle written in C
// by Hugo Burton
// CSSE2310 Assignment 1 Due 1st April 2022
// Student Number: s4698512

// #includes
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <csse2310a1.h>
#include <ctype.h>

// Definitions
#define MAX_BUFFER_SIZE 52

// Enums
// change tab stop to 8
/*
enum Alphabet {
	'a',
	'b',
	'c',
	'd',
	'e',
	'f',
	'g',
	'h',
	'i',
	'j',
	'k',
	'l',
	'm',
	'n',
	'o',
	'p',
	'q',
	'r',
	's',
	't',
	'u',
	'v',
	'w',
	'x',
	'y',
	'z'
}
*/

/*
 * Main function which handles running the program.
*/
int main(int argc, char* argv[]) {
	// Check for command line arguments
	int len = 5;	// Default value: 5
	int max = 6;	// Default value: 6

	int lenPresent = 0;
	int maxPresent = 0;
	
	const int lenMin = 3, maxMin = 3;
	const int lenMax = 9, maxMax = 9;

	const int maxArgs = 6;
	
	//char* dictPath = (char*)calloc(1, sizeof(char));
	char dictPath[MAX_BUFFER_SIZE] = "\0";
	FILE* dicFile;

	int gameover = 0;
	int correct = 0;

	// Define the dictList as size 0 for now
	//char** dictList = (char**)malloc(0 * sizeof(char*));
	char** dictList = (char**)calloc(1, sizeof(char*));
	int dictSize = 0;
	int dicPresent = 0;
	char* defDic = "/usr/share/dict/words\0";

	// Alphabet
	char alphabet[26];
	for(int i = 0; i < 26; i++){
    	alphabet[i] = (char)(97 + i);
	}

	char* match = (char*)calloc(1, sizeof(char));	// '-', lowercase, UPPERCASE

	
	// Check arguments
	
	if (argc > maxArgs) {
		fprintf(stderr, "Too many arguments present");
		return 1;
	}

	for (int i = 1; i < argc; i++) {
		// If argument starts with a '-'
		if (argv[i][0] == '-') {
			if (dicPresent) {
				fprintf(stderr, "Argument error: dic must be last argument.\n");
				return 1;
			}
			// Check for -len
			if (!strcmp(argv[i], "-len")) {
				// Check if argument already given
				if (lenPresent == 1) {
					fprintf(stderr, "Argument error: %s already given.\n", 
							argv[i]);
					return 1;
				}
				
				lenPresent = 1;
				//printf("Argument: %s\n", argv[i]);	// Debug
				// Check for subsequent length
				
				// if atoi fails, it will return 0 which will result in error 1
				len = atoi(argv[i + 1]);
				if (len >= lenMin && len <= lenMax) {
					//printf("Length value: %d\n", len);	// Debug
					// Skip next loop as we are checking here
					i++;
				} else {
					printf("Invalid %s argument value: %d. Value must be "
							"between %d and %d", argv[i], len, lenMin, 
							lenMax);
					/*fprintf(stderr, "invalid %s argument value: %d. Value " + 
							"must be between %d and %d.", argv[i], len, 
							lenMin, lenMax);*/
					return 1;
				}
			} else if (!strcmp(argv[i], "-max")) { // Check for -max
				if (maxPresent == 1) {
					fprintf(stderr, "Argument error: %s already given.\n", 
							argv[i]);
					return 1;
				}
				
				maxPresent = 1;
				//printf("Argument: %s\n", argv[i]);	// Debug
				// Check for subsequent max value
				
				// if atoi fails, it will return 0 which will result in error 1
				max = atoi(argv[i + 1]);
				if (max >= maxMin && max <= maxMax) {
					//printf("Max value: %d\n", max);	// Debug
					// Skip next loop as we are checking here
					i++;
				} else {
					/*fprintf(stderr, "invalid %s argument value: %d. Value " +
							"must be between %d and %d.", argv[i], max, maxMin, 
							maxMax);*/
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
			// Else case will only happen if dictionary path is provided
			// Check for exisiting dictionary path
			if (!strcmp(dictPath, "")) {
				// Size dictPath for size of path passed as argument
				//dictPath = realloc(dictPath, (strlen(argv[i])+1) * sizeof(char));
				strcpy(dictPath, argv[i]);
				dicPresent = 1;
			} else {
				fprintf(stderr, "Too many arguments given: %s not expected\n",
						argv[i]);
				return 1;
			}
		}
	}
	
	if (!strcmp(dictPath, "")) {
		// No dictionary has been provided
		// Set default path
		//dictPath = (char*)realloc(dictPath, sizeof(char) * strlen(defDic));
		strcpy(dictPath, defDic);
	}
	
	// Debug print of Dictionary
	printf("Dictionary path: %s\n", dictPath);
	
	// Check given dictionary path is valid
	// Open file for read only
	dicFile = fopen(dictPath, "r");
	if (dicFile) {
		// File can be opened
		
		char s[MAX_BUFFER_SIZE];
		// Loop through dictionary file until we reach EOF or fgets == NULL
		while (fgets(s, MAX_BUFFER_SIZE, dicFile) != NULL) {
			strtok(s, "\n"); // Remove \n at the end of s
			if (strlen(s) == len) {	
				// Only import words of the correct length
				// Far more optimised
				dictList = realloc(dictList, ++dictSize * sizeof(char*));
				dictList[dictSize - 1] = (char*)malloc(MAX_BUFFER_SIZE);
				strcpy(dictList[dictSize - 1], s);
			}
		}

		fclose(dicFile); 	// Close dictionary as it's not needed anymore
	} else /* (dicFile == NULL) */ {
		// If the dictionary file could not be opened
		fprintf(stderr, "wordle: dictionary file \"%s\" cannot be "
				"opened\n", dictPath);
	}

	
	//char* correctWord = get_random_word(len);
	
	char* correctWord = get_random_word(len);
	strtok(correctWord, "\n");
	printf("Correct Word: %s\n", correctWord);
	// At this point, all arguments have been dealt with
	// Welcome message
	
	printf("----------Welcome to wordle.c----------\n\n");
	// Prompt for input
	char input[MAX_BUFFER_SIZE];
	int guesses = 0;
	
	while(!gameover) {
		if (guesses >= max) {
			// Guesses exceeded max
			gameover = 1;
			break;
		}
		
		// ----------Input prompt---------- //
		printf("Enter a %d letter word (", len);
		max - guesses == 1 ? printf("last attempt") : 
			printf("%d attempts remaining", max - guesses);
		printf("):\n");

		if (fgets(input, MAX_BUFFER_SIZE, stdin) == NULL) {
			// Invalid input. Try again
			fprintf(stderr, "Invalid input was detected. Please try again.\n");
			continue;
		}

		// Strip '\n' from the end
		strtok(input, "\n");

		// ------Check guess------ //

		// Correct length
		//printf("strlen(input: %s) == %ld\n", input, strlen(input)); 
		if (strlen(input) != len) {
			// If string length is not correct
			printf("Words must be %d letters long - try again.\n", len);
			continue;
		}

		// Check alpha
		int nonAlphaDet = 0;
		for (int i = 0; i < len; i++) {
			if (!isalpha(input[i]) && !nonAlphaDet) {
				printf("Words must contain only letters - try again.\n");
				nonAlphaDet = 1;
				break;
			}
		}

		if (nonAlphaDet) {
			continue;
		}
		
		// Convert to lower case for comparison
		for (int i = 0; i < len; i++) {
			input[i] = tolower(input[i]);
		}
		
		// Check for valid word in Dictionary
		int validWord = 0;

		for (int i = 0; i < dictSize; i++) {
			//printf("Input --> %s == %s <-- dictList[i]\n", input, dictList[i]);
			if (strcmp(input, dictList[i]) == 0) {
				// Found valid word
				validWord = 1;
				break;
			}
		}

		if (!validWord) {
			printf("Word not found in the dictionary - try again.\n");
			continue;
		}

		// Get instances of each char in the word
		
		int inputHisto[26] = { 0 };
		int correctHisto[26] = { 0 };
		
		for (int ai = 0; ai < 26; ai++) {
			// input
			for (int i = 0; i < len; i++) {
				//printf("Sizeof input look: %d\n", i);
				if (input[i] == alphabet[ai]) {
					inputHisto[ai]++;
				}
			}
			
			// correctWord
			for (int i = 0; i < len; i++) {
				if (correctWord[i] == alphabet[ai]) {
					correctHisto[ai]++;
				}
			}
		}
		
		match = (char*)realloc(match, len * sizeof(char));
		
		for (int i = 0; i < len; i++) {
			match[i] = '-';
		}
		
		// Iterate over input
		for (int i = 0; i < len; i++) {
			// Check for exact match
			if (input[i] == correctWord[i]) {
				match[i] = toupper(correctWord[i]);
			}
			// Iterate over correctWord to check for partial match
			for (int j = 0; j < len; j++) {
				if (input[i] == correctWord[j] && i != j && 
						!isupper(match[i])) {
					// Correct char in wrong location
					match[i] = tolower(correctWord[j]);
				}
			}
		}
		
		// Loop in reverse over match and remove excess lower case
		// This is because we only want to display the first cases where
		// duplicates are present
		for (int ai = 0; ai < 26; ai++) {
			if (correctHisto[ai] > 0) {
				while (inputHisto[ai] > correctHisto[ai]) {
					for (int j = strlen(match) - 1; j >= 0; j--) {
						//printf("j = %d\n", j);
						if (tolower(match[j]) == alphabet[ai]) {
							inputHisto[ai]--;
							if (match[j] == alphabet[ai]) {
								match[j] = '-';
								j = 0;
							}
						}
					}
				}
			}
		}
		
		int fullyCorrect = 1;
		// Correct guess
		for (int i = 0; i < len; i++) {
			if (match[i] != toupper(correctWord[i])) {
				fullyCorrect = 0;
			}
		}

		if (fullyCorrect) {
			printf("Correct!\n");
			correct = 1;
			free(correctWord);
			break;
		}
	
		// Print match
		printf("%s\n", match);

		// Reset Match
		for (int i = 0; i < len; i++) {
			match[i] = '-';
		}

		guesses++;
	}
	
	if (!correct) {
		printf("Bad luck - the word is \"%s\"\n", correctWord);
	}
	
	// ------Cleaning up variables------
	
	// Dictionary array
	
	for (int i = 0; i < dictSize; i++) {
		free(dictList[i]);
	}
	free(dictList);
	free(match);
	
	// Clean exit of program
	return 0;
}

int checkDictionary(char* dict[]) {
	return 0;
}


/* returns -1 if c is not an alphabetic character */
int c_to_n(char c) {
	int n = -1;
	static const char * const alphabet = "abcdefghijklmnopqrstuvxyz";
	char *p = strchr(alphabet, toupper((unsigned char)c));
	
	if (p) {
		n = p - alphabet;
	}

    return n;
}
