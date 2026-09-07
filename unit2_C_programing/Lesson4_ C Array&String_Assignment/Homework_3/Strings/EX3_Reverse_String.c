#include <stdio.h>
#include <string.h>

int main(void)
{
    char text[1000], temporary;
    int left = 0;

    printf("Enter the string: ");
    fgets(text, sizeof text, stdin);
    text[strcspn(text, "\n")] = '\0';
    int right = (int)strlen(text) - 1;

    while (left < right) {
        temporary = text[left];
        text[left] = text[right];
        text[right] = temporary;
        ++left;
        --right;
    }
    printf("Reverse string is: %s\n", text);
    return 0;
}
