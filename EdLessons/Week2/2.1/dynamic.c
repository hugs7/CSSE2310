#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) {
    int numElements = 0;    // Number of elements in the array
    // Declare and initialise an array to hold one integer.
    int* arr = (int*)malloc(sizeof(int) * 1);
    // Add argc to the array
    arr[numElements] = argc;
    numElements++;

    char buffer[80];
    char* line;
    
    // this is equivalent to "while fgets doesn't return NULL"
    while ((line = fgets(buffer, 80, stdin))){
        // If the value is 0 or Not a Number then stop reading numbers, 
        // i.e. break out of the loop
        int number = atoi(line);

        if (number == 0) {
            break;
        }

        // Reallocate space for the array to store 1 more int
        arr = (int* )reallocarray(arr, sizeof(int), numElements + 1);
        // Put the number into the array
        arr[numElements] = number;
        // Update the count of the number of elements in the array
        numElements++;
    }
    
    // Print out the numbers from the array in reverse order
    for (int i = numElements - 1; i >= 0; i--) {
        printf("%d\n", arr[i]);
    }
    // Free memory allocated to store the array
    free(arr);
    return 0;
}
