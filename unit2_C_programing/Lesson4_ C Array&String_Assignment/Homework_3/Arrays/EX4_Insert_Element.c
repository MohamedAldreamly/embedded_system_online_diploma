#include <stdio.h>

int main(void)
{
    int array[30], element, count, index, location;

    printf("Enter number of elements: ");
    scanf("%d", &count);
    if (count < 0 || count >= 30) {
        printf("Number of elements must be between 0 and 29.\n");
        return 1;
    }
    for (index = 0; index < count; ++index)
        scanf("%d", &array[index]);

    printf("Enter the element to be inserted: ");
    scanf("%d", &element);
    printf("Enter the location: ");
    scanf("%d", &location);
    if (location < 1 || location > count + 1) {
        printf("Invalid location.\n");
        return 1;
    }

    for (index = count; index >= location; --index)
        array[index] = array[index - 1];
    array[location - 1] = element;
    ++count;

    printf("Result:");
    for (index = 0; index < count; ++index)
        printf(" %d", array[index]);
    printf("\n");
    return 0;
}
