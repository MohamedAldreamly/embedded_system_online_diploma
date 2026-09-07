#include <stdio.h>

int main(void)
{
    float number1, number2, product;

    printf("Enter two numbers: ");
    scanf("%f %f", &number1, &number2);

    product = number1 * number2;
    printf("Product: %.2f\n", product);

    return 0;
}
