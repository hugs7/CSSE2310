#include <stdio.h>

/* Holds identity information for a person. */
struct Person {
    char* name;
    int age;
    float mass;
    float height;
};

int main(int argc, char** argv) {
    struct Person person;
    person.name = "John";
    person.age = 21;
    person.mass = 72.0;     //kg
    person.height = 1.82;   //m
    
    printf("%s is age %d with a mass and height of %gkg and %gm respectively\n", 
            person.name, person.age, person.mass, person.height);
    
    return 0;
}
