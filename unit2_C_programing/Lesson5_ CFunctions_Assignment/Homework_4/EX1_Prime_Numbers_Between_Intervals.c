#include <stdio.h>

int is_prime(int number)
{
    if (number < 2)
        return 0;
    for (int divisor = 2; divisor <= number / divisor; ++divisor)
        if (number % divisor == 0)
            return 0;
    return 1;
}

int main(void)
{
    int start, end;
    printf("Enter two numbers (intervals): ");
    scanf("%d %d", &start, &end);

    if (start > end) {
        int temporary = start;
        start = end;
        end = temporary;
    }

    printf("Prime numbers between %d and %d are: ", start, end);
    for (int number = start + 1; number < end; ++number)
        if (is_prime(number))
            printf("%d ", number);
    printf("\n");
    return 0;
}
