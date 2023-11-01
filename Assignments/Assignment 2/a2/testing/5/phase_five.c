#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char* argv[]) {
    //char* mystring = "oliver hugo whatever";
    char* dummy = calloc(26, sizeof(char));
    strcpy(dummy, "abcdefghijklmnoprstuvwxyz");
    
    char* phase5string = dummy + 13;
    
    printf("%s\n", phase5string);

    return 0;
}
