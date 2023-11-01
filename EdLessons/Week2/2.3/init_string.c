#include <stdio.h>
#include <stdlib.h>

typedef struct String {
    char* array;
    int size;
} Sttring;

/*
 * Returns the number of characters in string up to the null terminator ('\0').
**/
int my_str_len(char* string) {
    int len = 0;
    while (string[len]) {
	len++;
    }
    return len;
}

/**
 * Copies the chars in orig char* to new char* which is returned
**/
char* my_mem_cpy(char* orig) {
    // Declae char array and allocate memory
    char* arr = (char*)malloc(sizeof(my_str_len(orig) + 1));
    // Copy each char in orig array to result array, including null terminator
    int i;
    for (i = 0; i < my_str_len(orig); ++i) {
	arr[i] = orig[i];
    }
    arr[i + 1] = '\0';
    // Return new array
    return arr;
}

/*
 * Initialises a new String pointer populated based on the char* provided
**/
String* init_string(char* string) {
    // Initialise String pointer
   String* str = malloc(sizeof(String));
   // Declare property values
   str->array = my_mem_cpy(string);
   str->size = my_str_len(string);
   // Return string created
   return str;
}

int main(void) {
    String string = init_string("test");
    
    printf("%s\n", string->array);
    printf("%s\n", string->size);

    return 0;
}
