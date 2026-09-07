#include <stdio.h>

int main(void)
{
    int number;
    long long sum = 0;

    printf("Enter an integer: ");
    scanf("%d", &number);

    if (number < 1) {
        printf("Please enter a positive integer.\n");
        return 1;
    }

    for (int i = 1; i <= number; ++i)
        sum += i;

    printf("Sum = %lld\n", sum);
    return 0;
}
