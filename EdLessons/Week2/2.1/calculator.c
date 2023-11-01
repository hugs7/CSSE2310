#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int do_operation(int num1, char operation, int num2){
    if (operation == '-'){
        return num1 - num2;
    } else if (operation == '+') {
        return num1 + num2;
    } else if (operation == '*') {
        return num1 * num2;
    } else if (operation == '/') {
        return num1 / num2;
    } else {
        exit(3);
    }
}

int main(int argc, char** argv) {
    // Check number of command line arguments is correct
    if (argc < 3) {
        return 1;
    }
    
    char operator = argv[1][0]; // take the first character (index 0) as the
                                // operator - ignore other chars, if any
    int num1 = atoi(argv[2]); // atoi convert a string to an integer
    
    int num2 = atoi(argv[3]);// Finish this line
    
    // Validate that the nums are positive

    if (num1 <= 0 || num2 <= 0) {
        return 2;
    }

    int result = do_operation(num1, operator, num2);
    
    // Fill this in
    printf("%d %c %d = %d\n", num1, operator, num2, result);

    return 0;
}


