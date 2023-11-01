#include <stdio.h>

// Modify the struct declaration to typedef it
typedef struct String {
    char* array;
    int size;
} String;

/*
 * Returns the number of characters in string up to the null character ('\0').
*/
int my_str_len(char* string) {
    int len = 0;
    while (string[len]) {
	len++;
    }
    return len;
}

int main(void) {
    String string;
    string.size = 0;
    string.array = "";

    return 0;
}
