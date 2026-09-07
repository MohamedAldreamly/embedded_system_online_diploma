#include <stdio.h>

long long power(int base, unsigned int exponent)
{
    if (exponent == 0)
        return 1;
    return base * power(base, exponent - 1);
}

int main(void)
{
    int base, exponent;
    printf("Enter base number: ");
    scanf("%d", &base);
    printf("Enter power number (positive integer): ");
    scanf("%d", &exponent);
    if (exponent < 0) {
        printf("Exponent must be non-negative.\n");
        return 1;
    }
    printf("%d^%d = %lld\n", base, exponent,
           power(base, (unsigned int)exponent));
    return 0;
}
