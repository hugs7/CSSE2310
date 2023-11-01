#include <stdio.h>
#include <string.h>

#define MAX_BUFFER_SIZE 80

/* Prompts user for password, waits for correct password */
void wait_for_correct_password(void) {
    char buffer[MAX_BUFFER_SIZE];

    char* password;
    
    printf("Enter password: ");

    do {
        password = fgets(buffer, MAX_BUFFER_SIZE, stdin);
        
    } while (strcmp(password, "password1\n") != 0);

    printf("Password correct!\n");
}

int main(int argc, char** argv){
    wait_for_correct_password();
}
