#include <stdio.h>
#include <string.h>

int main(void)
{
    char text[1000];
    int length;

    printf("Enter a string: ");
    fgets(text, sizeof text, stdin);
    text[strcspn(text, "\n")] = '\0';
    for (length = 0; text[length] != '\0'; ++length)
        ;
    printf("Length of string: %d\n", length);
    return 0;
}
