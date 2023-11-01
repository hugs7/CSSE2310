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

#define WELCOME "Welcome to Wordle!"

#define CORRECT "Correct!"

#define E_INVALID_ARGUMENTS "Usage: wordle [-len word-length] [-max " \
    "max-guesses] [dictionary]\n"
#define E_INVALID_DICT_PATH(X) "wordle: dictionary file \"X\" cannot be opened"


// Structs
typedef struct Dictionary {
    char** list;
    int size;
} Dictionary;

// --------Function Declarations--------
int check_arguments(int argc, char** argv, int* len, int* max, char* dictPath);

int read_dictionary(char* dictPath, Dictionary* dict, int len);

int game_loop(char* correctWord, char* input, int* guesses, char* match, 
        Dictionary* dict, int len, int max);

// Helper Functions

int is_all_digits(char* s);

int c_to_n(char c);

char alphabet(int i);

char* set_string(char* s, int len, char c);

// --------Main program--------

int main(int argc, char* argv[]) {
    // Variables
    int len = 5, max = 6;   // Set default values
    
    Dictionary dict;
    char dictPath[MAX_BUFFER_SIZE] = "\0";
    dict.list = (char**)calloc(1, sizeof(char*)), dict.size = 0;

    char* input = (char*)calloc(MAX_BUFFER_SIZE, sizeof(char));
    char* match = (char*)calloc(1, sizeof(char));   // '-', lower, UPPER
    int guesses = 0, gameover = 0, readStatus;

    // Check arguments
    int args = check_arguments(argc, argv, &len, &max, dictPath);
    if (args) {
    	return 1;
    }

    match = (char*)realloc(match, (len + 1) * sizeof(char));
	
    // Read dictionary
    readStatus = read_dictionary(dictPath, &dict, len);
    if (readStatus) {
    	return readStatus;
    }
	
    // Get random word
    char* correctWord = get_random_word(len);
    strtok(correctWord, "\n");
    
    // Welcome message	
    printf("%s\n", WELCOME);
    
    do {
    	gameover = game_loop(correctWord, input, &guesses, match, &dict, len, 
                max);
        if (gameover == 0) {
            guesses++;
    	}
    } while (gameover <= 1);
    	
    // Free variables
    for (int i = 0; i < dict.size; i++) {
    	free(dict.list[i]);
    }
    free(input);
    free(dict.list);
    free(match);

    if (gameover != 2) {
    	fprintf(stderr, "Bad luck - the word is \"%s\".\n", correctWord);
        free(correctWord);
    	return 3;
    }
    free(correctWord);
    
    return 0; // Clean exit of program
}

/*
 * check_arguments()
 * ------------------
 * Checks arguments passed to the program. If criteria is not met, function 
 * will return an error.
 *
 * argc: number of arguments passed to the program. Should be less than 
 *       maxArgs however, function will deal with a greater number if passed.
 * argv: array of array of characters (array of Strings) containing the 
 * 	 arguments passed to the program. These are checked in this function.
 *
 * Returns: an integer which corresponds to the validity of arguments passed
 *          to the program. Will return 0 if valid arguments and !0 if 
 *          invalid arguments.
 */
int check_arguments(int argc, char** argv, int* len, int* max, 
        char* dictPath) {
    // Some local scope variables
    int lenPresent = 0;
    int maxPresent = 0;
    int dictPresent = 0;
	
    const int lenMin = 3, maxMin = 3;
    const int lenMax = 9, maxMax = 9;

    const int maxArgs = 6;
    int digits = 0;

    // Check number of arguments	
    if (argc > maxArgs) {
    	fprintf(stderr, E_INVALID_ARGUMENTS);
    	return 1;
    }

    // Loop through number of arguments
    // (except the first argument which is always ./wordle
    for (int i = 1; i < argc; i++) {
    	// If argument starts with a '-'
    	if (argv[i][0] == '-') {
    	    if (dictPresent) {
    	    	fprintf(stderr, E_INVALID_ARGUMENTS);
    	    	return 1;
    	    }
    	    // Check for -len
    	    if (!strcmp(argv[i], "-len")) {
    	        // Check if argument already given
    	    	if (lenPresent == 1) {
    	            fprintf(stderr, E_INVALID_ARGUMENTS);
    		    return 1;
    		}
		
	    	lenPresent = 1;
		// Check for subsequent length
		
	        // Check if next argv exists
		if (argc < i + 2) {
		    fprintf(stderr, E_INVALID_ARGUMENTS);
		    return 1;
		}
		// Check if all digits
		digits = is_all_digits(argv[i + 1]);
		if (digits) {
		    fprintf(stderr, E_INVALID_ARGUMENTS);
		    return 1;
		}
		
		// if atoi fails, it will return 0 which will result in error 1
		*len = atoi(argv[i + 1]);
		if (*len >= lenMin && *len <= lenMax) {
		    // Skip next loop as we are checking here
		    i++;
		} else {
		    fprintf(stderr, E_INVALID_ARGUMENTS);
		    return 1;
                }
	    } else if (!strcmp(argv[i], "-max")) { // Check for -max
	        if (maxPresent == 1) {
	    	    fprintf(stderr, E_INVALID_ARGUMENTS);
	    	    return 1;
		}
                maxPresent = 1;
		// Check for subsequent max value

	        // Check if next argv exists
		if (argc < i + 2) {
		    fprintf(stderr, E_INVALID_ARGUMENTS);
		    //fprintf(stderr, "Error arguments\n");
		    return 1;
		}
			
		// Check if all digits
		digits = is_all_digits(argv[i + 1]);
		if (digits) {
		    fprintf(stderr, E_INVALID_ARGUMENTS);
		    return 1;
		}
		
		// if atoi fails, it will return 0 which will result in error 1
		*max = atoi(argv[i + 1]);
		if (*max >= maxMin && *max <= maxMax) {
		    i++;
		} else {
		    fprintf(stderr, E_INVALID_ARGUMENTS);
		    return 1;
		}
	    } else {
		// Else if an argument starts with '-' but is not -len or -max
		// then program errors.
		fprintf(stderr, E_INVALID_ARGUMENTS);
		return 1;
	    }
				
	} else if (!strcmp(argv[i], "")) {
	    // Any argument is the empty string.
	    fprintf(stderr, E_INVALID_ARGUMENTS);
	    return 1;
	} else {
	    // Else case will only happen if dictionary path is provided
	    // Check for exisiting dictionary path
	    if (!strcmp(dictPath, "")) {
		// Size dictPath for size of path passed as argument
		strcpy(dictPath, argv[i]);
		dictPresent = 1;
	    } else {
		fprintf(stderr, E_INVALID_ARGUMENTS);
		return 1;
	    }
	}
    }

    return 0;
}

/*
 * read_dictionary
 *
 * takes a dictionary path and reads the file. Puts contents with len into
 * the passed struct dict
 *
 * args: dictPath the path to the dictionary file
 * dict the dictionary variable structure
 * len the length of words passed
 * returns status code as int
 */
int read_dictionary(char* dictPath, Dictionary* dict, int len) {
    // Variables
	
    const char* defaultPath = "/usr/share/dict/words\0";

    if (!strcmp(dictPath, "")) {
	// No dictionary has been provided
	// Set default path
	strcpy(dictPath, defaultPath);
    }
	
    // Debug print of Dictionary
    //printf("Dictionary path: %s\n", dictPath);
	
    // Check given dictionary path is valid
    // Open file for read only
    FILE* dicFile = fopen(dictPath, "r");
    if (dicFile) {
	// File can be opened
		
	char s[MAX_BUFFER_SIZE];
	// Loop through dictionary file until we reach EOF or fgets == NULL
	while (fgets(s, MAX_BUFFER_SIZE, dicFile) != NULL) {
	    strtok(s, "\n"); // Remove \n at the end of s
	    if ((int)strlen(s) == len) {	
		// Only import words of the correct length
		// Far more optimised
		dict->list = realloc(dict->list, 
                        ++(dict->size) * sizeof(char*));
		dict->list[dict->size - 1] = (char*)calloc(MAX_BUFFER_SIZE, 
                        sizeof(char));
		strcpy(dict->list[dict->size - 1], s);
	    }
	}

        fclose(dicFile); 	// Close dictionary as it's not needed anymore
    } else {     /* (dicFile == NULL) */
	// If the dictionary file could not be opened
	fprintf(stderr, "wordle: dictionary file \"%s\" cannot be "
                "opened\n", dictPath);
	return 2;
    }
    
    return 0;
}

/*
 * Main game loop
 */
int game_loop(char* correctWord, char* input, int* guesses, char* match, 
        Dictionary* dict, int len, int max) {
    if (*guesses >= max) {
	// Guesses exceeded max
	// Game over
	return 3;
    }
	
    // ----------Input prompt---------- //
    printf("Enter a %d letter word (", len);
    max - *guesses == 1 ? printf("last attempt") : 
    printf("%d attempts remaining", max - *guesses);
    printf("):\n");
    
    if (fgets(input, MAX_BUFFER_SIZE, stdin) == NULL) {
	// Invalid input. Try again
	return 3;
    }

    // Strip '\n' from the end
    strtok(input, "\n");

    // ------Check guess------ //
    
    // Correct length
    if ((int)strlen(input) != len) {
	// If string length is not correct
	printf("Words must be %d letters long - try again.\n", len);
	return 1;
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
	return 1;
    }
    
    // Convert to lower case for comparison
    for (int i = 0; i < len; i++) {
	input[i] = tolower(input[i]);
    }
    
    // Get instances of each char in the word
	
    int inputHisto[26] = { 0 };
    int correctHisto[26] = { 0 };

    for (int ai = 0; ai < 26; ai++) {
	// input
	for (int i = 0; i < len; i++) {
	    if (input[i] == alphabet(ai)) {
		inputHisto[ai]++;
	    }
        }
		
	// correctWord
	for (int i = 0; i < len; i++) {
	    if (correctWord[i] == alphabet(ai)) {
		correctHisto[ai]++;
	    }
	}
    }
    
    match = set_string(match, len, '-');
    
    // Iterate over input
    for (int i = 0; i < len; i++) {
        // Check for exact match
	if (input[i] == correctWord[i]) {
	    match[i] = toupper(correctWord[i]);
	}
	// Iterate over correctWord to check for partial match
	for (int j = 0; j < len; j++) {
	    if (input[i] == correctWord[j] && i != j && !isupper(match[i])) {
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
		for (int j = len - 1; j >= 0; j--) {
		    //printf("j = %d\n", j);
		    if (tolower(match[j]) == alphabet(ai)) {
			inputHisto[ai]--;
			if (match[j] == alphabet(ai)) {
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
	printf("%s\n", CORRECT);
	return 2;   // Finish game loop
    }

    // Check for valid word in Dictionary
    int validWord = 0;
    for (int i = 0; i < dict->size; i++) {
	if (strcmp(input, dict->list[i]) == 0) {
	    // Found valid word
	    validWord = 1;
	    break;
	}
    }
    
    if (!validWord) {
	printf("Word not found in the dictionary - try again.\n");
	return 1;
    }

    // Print match
    printf("%s\n", match);
    
    // Reset Match
    match = set_string(match, len, '-');
    return 0;      // keep going through the loop
}

// -----Helper Functions-----

/*
 * is_all_digits()
 *
 * Checks that a string is all digits (so that it can be converted
 * to an int)
 *
 * returns 0 if all digits
 * returns 1 if a non-digit character is detected
 */
int is_all_digits(char* s) {
    for (int i = 0; i < (int)strlen(s); i++) {
	if (!isdigit(s[i])) {
	    return 1;
	}
    }
    return 0;
}

/* 
 * c_to_n()
 * returns -1 if c is not an alphabetic character 
 */
int c_to_n(char c) {
    int n = -1;
    static const char* const alphabet = "abcdefghijklmnopqrstuvxyz";
    char* p = strchr(alphabet, toupper((unsigned char)c));
	
    if (p) {
    	n = p - alphabet;
    }

    return n;
}

/*
 * Takes an int i from 0 to 25 and returns the alphabetic char
**/
char alphabet(int i) {
    return (char)(97 + i);
}

/*
 * set_string(char* s, int len, char c)
 * Takes a string (char* s) and sets all chars to character given (char c)
 * len is the length of the string s. This is not checked in this function
 * returns string with all chars set to c
 */
char* set_string(char* s, int len, char c) {
    for (int i = 0; i < len; i++) {
        s[i] = c;
    }
    s[len] = '\0';
    return s;
}
