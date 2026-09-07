#include <stdio.h>

int main(void)
{
    char operation;
    double number1, number2;

    printf("Enter operator either + or - or * or divide: ");
    scanf(" %c", &operation);
    printf("Enter two operands: ");
    scanf("%lf %lf", &number1, &number2);

    switch (operation) {
    case '+':
        printf("%.1f + %.1f = %.1f\n", number1, number2, number1 + number2);
        break;
    case '-':
        printf("%.1f - %.1f = %.1f\n", number1, number2, number1 - number2);
        break;
    case '*':
        printf("%.1f * %.1f = %.1f\n", number1, number2, number1 * number2);
        break;
    case '/':
        if (number2 == 0) {
            printf("Error: division by zero is not allowed.\n");
            return 1;
        }
        printf("%.1f / %.1f = %.1f\n", number1, number2, number1 / number2);
        break;
    default:
        printf("Error: invalid operator.\n");
        return 1;
    }

    return 0;
}
