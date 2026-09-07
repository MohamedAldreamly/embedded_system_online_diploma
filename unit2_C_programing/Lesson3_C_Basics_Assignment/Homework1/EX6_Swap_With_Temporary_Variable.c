#include <stdio.h>

int main(void)
{
    float a, b, temporary;

    printf("Enter value of a: ");
    scanf("%f", &a);
    printf("Enter value of b: ");
    scanf("%f", &b);

    temporary = a;
    a = b;
    b = temporary;

    printf("After swapping, value of a = %.2f\n", a);
    printf("After swapping, value of b = %.2f\n", b);

    return 0;
}
