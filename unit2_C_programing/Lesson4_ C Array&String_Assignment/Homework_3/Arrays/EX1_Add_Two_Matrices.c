#include <stdio.h>

int main(void)
{
    float a[2][2], b[2][2], sum[2][2];
    int row, column;

    printf("Enter the elements of 1st matrix\n");
    for (row = 0; row < 2; ++row)
        for (column = 0; column < 2; ++column) {
            printf("Enter a%d%d: ", row + 1, column + 1);
            scanf("%f", &a[row][column]);
        }

    printf("Enter the elements of 2nd matrix\n");
    for (row = 0; row < 2; ++row)
        for (column = 0; column < 2; ++column) {
            printf("Enter b%d%d: ", row + 1, column + 1);
            scanf("%f", &b[row][column]);
        }

    for (row = 0; row < 2; ++row)
        for (column = 0; column < 2; ++column)
            sum[row][column] = a[row][column] + b[row][column];

    printf("\nSum of Matrix:\n");
    for (row = 0; row < 2; ++row) {
        for (column = 0; column < 2; ++column)
            printf("%.1f\t", sum[row][column]);
        printf("\n");
    }
    return 0;
}
