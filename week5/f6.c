#include <stdio.h>
#include <stdlib.h>

void bye(void) {
    printf("Leaving now");
}

int main(int argc, char** argv) {
    atexit(bye);
    return 5;
}
