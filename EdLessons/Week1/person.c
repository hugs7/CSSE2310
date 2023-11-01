#include <stdio.h>

/*
** Prints the given personal details of a person.
*/ 
void print_person(char* name, int age) {
    printf("Person's name: %s\nFirst letter of person's name: %c\nPerson's age in hexadecimal: %x\n", name, name[0], age);
}

int main(void) {
    print_person("John", 21);
    print_person("Alice", 0);
    
    return 0;
}
