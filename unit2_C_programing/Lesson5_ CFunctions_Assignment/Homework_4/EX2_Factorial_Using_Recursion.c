#include <stdio.h>

unsigned long long factorial(unsigned int number)
{
    if (number <= 1)
        return 1;
    return number * factorial(number - 1);
}

int main(void)
{
    int number;
    printf("Enter a positive integer: ");
    scanf("%d", &number);
    if (number < 0) {
        printf("Factorial of a negative number does not exist.\n");
        return 1;
    }
    printf("Factorial of %d = %llu\n", number, factorial((unsigned int)number));
    return 0;
}
