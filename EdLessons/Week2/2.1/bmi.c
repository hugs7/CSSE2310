#include <stdio.h>

/**
 * Return the BMI of a person with the specified mass (in kg) and
 * height (in meters).
**/
float compute_bmi(float mass, float height) {
    return mass / (height * height);
}

int main(void) {
    printf("%f\n", compute_bmi(75, 1.82)); 

    return 0;
}
