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

int check_len(int argc, char** argv, int* i, int* len, int lenMin, int lenMax, 
        int* lenPresent, int* digits); 

int check_max(int argc, char** argv, int* i, int* max, int maxMin, int maxMax, 
        int* maxPresent, int* digits);

// Helper Functions

int is_all_digits(char* s);

int c_to_n(char c);

char alphabet(int i);

void input_prompt(int len, int max, int guesses);

int check_guess(char* input, int len, int* inputHisto, int* correctHisto,
        char* correctWord);

void check_match(char* input, char* correctWord, char* match, 
        int* inputHisto, int* correctHisto, int len);

char* set_string(char* s, int len, char c);

char* string_to_lower(char* s, int len);

char* string_to_upper(char* s, int len);

void free_variables(Dictionary dict, char* input, char* match);

// --------Main program--------

int main(int argc, char* argv[]) {
    int len = 5, max = 6;   // Set default values
    Dictionary dict;
    char dictPath[MAX_BUFFER_SIZE] = "\0";
    dict.list = (char**)calloc(1, sizeof(char*)), dict.size = 0;
    char* input = (char*)calloc(MAX_BUFFER_SIZE, sizeof(char));
    char* match = (char*)calloc(1, sizeof(char));   // '-', lower, UPPER
    int guesses = 0, gameover = 0, readStatus;
    int args = check_arguments(argc, argv, &len, &max, dictPath);
    
    if (args) {
    	return 1;
    }
    match = (char*)realloc(match, (len + 1) * sizeof(char));
    readStatus = read_dictionary(dictPath, &dict, len);
    if (readStatus) {
    	return readStatus;
    }
    
    char* correctWord = get_random_word(len);
    strtok(correctWord, "\n");
    
    printf("%s\n", WELCOME);
    
    do {
    	gameover = game_loop(correctWord, input, &guesses, match, &dict, len, 
                max);
        if (gameover == 0) {
            guesses++;
    	}
    } while (gameover <= 1);
    	
    free_variables(dict, input, match);
    
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
    int lenPresent = 0, maxPresent = 0, dictPresent = 0, digits = 0;
    const int lenMin = 3, maxMin = 3, lenMax = 9, maxMax = 9, maxArgs = 6;
    if (argc > maxArgs) {
    	fprintf(stderr, E_INVALID_ARGUMENTS);
    	return 1;
    }
    for (int i = 1; i < argc; i++) {    // Loop through number of arguments
    	if (argv[i][0] == '-') {
    	    if (dictPresent) {
    	    	fprintf(stderr, E_INVALID_ARGUMENTS);
    	    	return 1;
    	    }
    	    if (!strcmp(argv[i], "-len")) {
                int lenCheck = check_len(argc, argv, &i, len, lenMin, lenMax, 
                        &lenPresent, &digits);

                if (lenCheck != 0) {
                    return lenCheck;
                }
	    } else if (!strcmp(argv[i], "-max")) { // Check for -max
	        int maxCheck = check_max(argc, argv, &i, max, maxMin, maxMax,
                        &maxPresent, &digits);

                if (maxCheck != 0) {
                    return maxCheck;
                }    
            } else {
		fprintf(stderr, E_INVALID_ARGUMENTS);
		return 1;
	    }
	} else if (!strcmp(argv[i], "")) {
	    fprintf(stderr, E_INVALID_ARGUMENTS);
	    return 1;
	} else {
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
	return 3;   // Maxed guesses. Game over
    }

    input_prompt(len, max, *guesses);
    if (fgets(input, MAX_BUFFER_SIZE, stdin) == NULL) {
	return 3;   // Invalid input. Try again
    }
    strtok(input, "\n");   // Remove last \n from input 
   
    int* inputHisto = (int*)calloc(26, sizeof(int));
    int* correctHisto = (int*)calloc(26, sizeof(int));
    int check = check_guess(input, len, inputHisto, correctHisto, 
            correctWord);
    if (check != 0) {
        free(inputHisto);
        free(correctHisto);
        return check;
    }

    match = set_string(match, len, '-');
    check_match(input, correctWord, match, inputHisto, correctHisto, len);
    free(inputHisto);
    free(correctHisto);

    char* upperCorrectWord = string_to_upper(correctWord, len + 1);
    
    if (!strcmp(match, upperCorrectWord)) {
	printf("%s\n", CORRECT);
        free(upperCorrectWord);
	return 2;   // Finish game loop
    }
    free(upperCorrectWord);

    int validWord = 0;
    for (int i = 0; i < dict->size; i++) {
	if (strcmp(input, dict->list[i]) == 0) {
	    validWord = 1;
	    break;
	}
    }

    if (!validWord) {
	printf("Word not found in the dictionary - try again.\n");
	return 1;
    }

    printf("%s\n", match);
    return 0;      // Keep going through the loop
}

/*
 * check_len()
 * Checks length argument passed to program
 */
int check_len(int argc, char** argv, int* i, int* len, int lenMin, int lenMax, 
        int* lenPresent, int* digits) {
    // Check if argument already given
    if (*lenPresent == 1) {
        fprintf(stderr, E_INVALID_ARGUMENTS);
        return 1;
    }
    
    *lenPresent = 1;
    // Check for subsequent length
    
    // Check if next argv exists
    if (argc < *i + 2) {
        fprintf(stderr, E_INVALID_ARGUMENTS);
        return 1;
    }
    // Check if all digits
    *digits = is_all_digits(argv[*i + 1]);
    if (*digits) {
        fprintf(stderr, E_INVALID_ARGUMENTS);
        return 1;
    }
    
    // if atoi fails, it will return 0 which will result in error 1
    *len = atoi(argv[*i + 1]);
    if (*len >= lenMin && *len <= lenMax) {
        // Skip next loop as we are checking here
        (*i)++;
    } else {
        fprintf(stderr, E_INVALID_ARGUMENTS);
        return 1;
    }

    return 0;
}

/*
 * check_max()
 *
 * Checks length argument passed to program
 */
int check_max(int argc, char** argv, int* i, int* max, int maxMin, int maxMax,
        int* maxPresent, int* digits) {
    if (*maxPresent == 1) {
        fprintf(stderr, E_INVALID_ARGUMENTS);
        return 1;
    }

    *maxPresent = 1;
    // Check for subsequent max value

    // Check if next argv exists
    if (argc < *i + 2) {
        fprintf(stderr, E_INVALID_ARGUMENTS);
        //fprintf(stderr, "Error arguments\n");
        return 1;
    }
            
    // Check if all digits
    *digits = is_all_digits(argv[*i + 1]);
    if (*digits) {
        fprintf(stderr, E_INVALID_ARGUMENTS);
        return 1;
    }
    
    // if atoi fails, it will return 0 which will result in error 1
    *max = atoi(argv[*i + 1]);
    if (*max >= maxMin && *max <= maxMax) {
        (*i)++;
    } else {
        fprintf(stderr, E_INVALID_ARGUMENTS);
        return 1;
    }
    
    return 0;
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
 * input_prompt()
 *
 * Prompts user for input
 */
void input_prompt(int len, int max, int guesses) {
    printf("Enter a %d letter word (", len);
    max - guesses == 1 ? printf("last attempt") : 
    printf("%d attempts remaining", max - guesses);
    printf("):\n");
}

/*
 * check_guess()
 *
 * takes input and checks it for valid input
 */
int check_guess(char* input, int len, int* inputHisto, int* correctHisto,
        char* correctWord) {
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
    
    input = string_to_lower(input, len);    // Convert to lower case
    
    // Get instances of each char in the word
	

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
    
    return 0;
}

/*
 * check_match()
 *
 * Generates the match string which compares input with correctWord
 */
void check_match(char* input, char* correctWord, char* match, 
        int* inputHisto, int* correctHisto, int len) {
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

/*
 * string_to_lower()
 *
 * Converts an entire string s to lowercase
 * Modifies original string
 */
char* string_to_lower(char* s, int len) {
    for (int i = 0; i < len; i++) {
        s[i] = tolower(s[i]);
    }
    return s;
}

/*
 * string_to_upper()
 *
 * Converts an entire string s to uppercase
 * Doesn't modify original string
 */
char* string_to_upper(char* s, int len) {
    char* copy = (char*)calloc(len, sizeof(char));
    strcpy(copy, s);
    for (int i = 0; i < len; i++) {
        copy[i] = toupper(copy[i]);
    }
    return copy;
}

/*
 * free_variables()
 *
 * Frees dynamic memory
 */
void free_variables(Dictionary dict, char* input, char* match) {
    for (int i = 0; i < dict.size; i++) {
        free(dict.list[i]);
    }
    free(input);
    free(dict.list);
    free(match);
}
