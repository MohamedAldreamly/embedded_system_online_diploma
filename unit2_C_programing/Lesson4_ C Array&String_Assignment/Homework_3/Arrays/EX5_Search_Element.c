#include <stdio.h>

int main(void)
{
    int array[30], element, count, index;

    printf("Enter number of elements: ");
    scanf("%d", &count);
    if (count < 1 || count > 30) {
        printf("Number of elements must be between 1 and 30.\n");
        return 1;
    }
    printf("Enter the values: ");
    for (index = 0; index < count; ++index)
        scanf("%d", &array[index]);

    printf("Enter the element to be searched: ");
    scanf("%d", &element);
    for (index = 0; index < count && element != array[index]; ++index)
        ;

    if (index < count)
        printf("Number found at location = %d\n", index + 1);
    else
        printf("Number not found\n");
    return 0;
}
