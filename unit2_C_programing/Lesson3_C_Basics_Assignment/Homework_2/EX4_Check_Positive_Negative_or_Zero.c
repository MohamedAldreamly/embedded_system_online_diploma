#include <stdio.h>

int main(void)
{
    double number;

    printf("Enter a number: ");
    scanf("%lf", &number);

    if (number > 0)
        printf("%.2f is positive.\n", number);
    else if (number < 0)
        printf("%.2f is negative.\n", number);
    else
        printf("You entered zero.\n");

    return 0;
}
