#include <stdio.h>

int main(void)
{
    int number;
    unsigned long long factorial = 1;

    printf("Enter an integer: ");
    scanf("%d", &number);

    if (number < 0) {
        printf("Error!!! Factorial of negative number doesn't exist.\n");
        return 0;
    }

    for (int i = 1; i <= number; ++i)
        factorial *= (unsigned long long)i;

    printf("Factorial = %llu\n", factorial);
    return 0;
}
