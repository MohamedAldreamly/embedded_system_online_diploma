#include <stdio.h>

int main(void)
{
    int count, index;
    float numbers[100], sum = 0.0f;

    printf("Enter the number of data: ");
    scanf("%d", &count);
    while (count > 100 || count <= 0) {
        printf("Error! Number should be in range 1 to 100.\n");
        printf("Enter the number again: ");
        scanf("%d", &count);
    }

    for (index = 0; index < count; ++index) {
        printf("%d. Enter number: ", index + 1);
        scanf("%f", &numbers[index]);
        sum += numbers[index];
    }
    printf("Average = %.2f\n", sum / count);
    return 0;
}
