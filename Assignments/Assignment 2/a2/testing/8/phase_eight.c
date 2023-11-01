#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char* argv[]) {
    char next_rchar[] = {'8', '.', 'o', '\\', 'G', '"', 'v', 'y', 'Q', ']', 
        'V', 'u', '(', '4', 'f', '?', 'Q', 'o', '_', 'A', '#', '*', '6', 'q',
        'X', '\'', 'Q', 'c', 'L', 'H', '*', '}', 'U', 'x', '[', '{', 'z', 'l',
        '2', 'f', 'd', 'g', 'w', 'k', '{', '^', 'E', 'M', 'h', '&', '*', '&', 
        '/', '?', '2', '"', 'F', 'c', '~', '-', ',', '*', '+', '`', '#'};
    
    int ptr = -1;
    int mute = 0;
    for (int i = 10; i < 75; i++) {
        if (!(i%7)) {
            ptr++;
        } else {
            continue;
        }
         
        ptr++;
        if (mute == 0) {
            printf("%c ", next_rchar[ptr]);
        }
        
        mute = 1 - mute;
    }
    printf("\n\n"); 
    return 0;
}
