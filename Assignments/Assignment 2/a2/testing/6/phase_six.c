#include <stdlib.h>
#include <stdio.h>
#include <string.h>


void phase_six() {
    int depth = 50;

    for (int i = 0; i <= 60; i++) {
        printf("%c", 48+3*depth%8);
        depth+=1;
        if (depth / 3 / 20 != 0) {
            printf("\nDepth: %d", depth);
            break;
        }
    }
    printf("\n");
}

int main(int argc, char** argv) {
    phase_six();
    return 0;
}
