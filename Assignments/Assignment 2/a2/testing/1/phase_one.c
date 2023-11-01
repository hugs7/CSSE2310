#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void phase_one() {
    int i = 0;
    int j = 125;

    for (i = 1; i < 14; i++) {
        printf("mute(%d)\n", i % 2);

        if (! (i % 2)) {
            printf("append(%d)\n", j);
        }
        j += 162;
    }
}

int main(int argc, char** argv) {
    phase_one();
    return 0;
}
