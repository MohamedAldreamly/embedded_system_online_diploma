#include <stdio.h>
#include <string.h>

int main(void)
{
    char text[1000], character;
    int count = 0;

    printf("Enter a string: ");
    fgets(text, sizeof text, stdin);
    text[strcspn(text, "\n")] = '\0';
    printf("Enter a character to find frequency: ");
    scanf("%c", &character);

    for (int index = 0; text[index] != '\0'; ++index)
        if (character == text[index])
            ++count;

    printf("Frequency of %c = %d\n", character, count);
    return 0;
}
