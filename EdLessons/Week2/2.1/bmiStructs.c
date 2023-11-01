#include <stdio.h>

struct Person {
    char* name;
    int age;
    float mass;
    float height;
};

/**
 * Return the BMI of a person based off the properties of the
 * person parameter.
**/
float compute_bmi(struct Person person) {
    return person.mass / (person.height * person.height);
}

int main(void) {
    struct Person personA = {
        .name = "John", .age = 21, .mass = 72, .height = 1.82
    };

    printf("%s's BMI is %f.\n", personA.name, compute_bmi(personA)); 

    return 0;
}
