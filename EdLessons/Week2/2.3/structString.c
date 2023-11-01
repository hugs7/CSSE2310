#include <stdio.h>

struct String {
    char* array;
    int size;
};

/*
 * Returns the number of characters in string up to the null terminator ('\0').
*/
int my_str_len(char* string) {
    int len = 0;
    while (string[len]) {
	len++;
    }
    return len;
}

int main(void) {
    struct String string;
    string.size = 0;
    string.array = "";

    return 0;

}
