#include <stdio.h>

int main(void)
{
    double number1, number2, number3, largest;

    printf("Enter three numbers: ");
    scanf("%lf %lf %lf", &number1, &number2, &number3);

    largest = number1;
    if (number2 > largest)
        largest = number2;
    if (number3 > largest)
        largest = number3;

    printf("Largest number = %.2f\n", largest);
    return 0;
}
