#include <stdio.h>

int main(void)
{
    int matrix[10][10], transpose[10][10];
    int rows, columns, row, column;

    printf("Enter rows and columns of matrix: ");
    scanf("%d %d", &rows, &columns);
    if (rows < 1 || rows > 10 || columns < 1 || columns > 10) {
        printf("Rows and columns must be between 1 and 10.\n");
        return 1;
    }

    printf("Enter elements of matrix:\n");
    for (row = 0; row < rows; ++row)
        for (column = 0; column < columns; ++column) {
            printf("Enter element a%d%d: ", row + 1, column + 1);
            scanf("%d", &matrix[row][column]);
        }

    for (row = 0; row < rows; ++row)
        for (column = 0; column < columns; ++column)
            transpose[column][row] = matrix[row][column];

    printf("\nEntered Matrix:\n");
    for (row = 0; row < rows; ++row) {
        for (column = 0; column < columns; ++column)
            printf("%d ", matrix[row][column]);
        printf("\n");
    }

    printf("\nTranspose of Matrix:\n");
    for (row = 0; row < columns; ++row) {
        for (column = 0; column < rows; ++column)
            printf("%d ", transpose[row][column]);
        printf("\n");
    }
    return 0;
}
