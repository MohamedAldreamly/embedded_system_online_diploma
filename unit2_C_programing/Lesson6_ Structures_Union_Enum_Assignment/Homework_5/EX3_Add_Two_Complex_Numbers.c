#include <stdio.h>

typedef struct {
    float real;
    float imaginary;
} Complex;

Complex add(Complex first, Complex second)
{
    Complex result;
    result.real = first.real + second.real;
    result.imaginary = first.imaginary + second.imaginary;
    return result;
}

int main(void)
{
    Complex first, second, result;
    printf("For 1st complex number\n");
    printf("Enter real and imaginary respectively: ");
    scanf("%f %f", &first.real, &first.imaginary);

    printf("\nFor 2nd complex number\n");
    printf("Enter real and imaginary respectively: ");
    scanf("%f %f", &second.real, &second.imaginary);

    result = add(first, second);
    printf("Sum = %.1f%+.1fi\n", result.real, result.imaginary);
    return 0;
}
