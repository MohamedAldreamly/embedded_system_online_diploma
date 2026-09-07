#include <stdio.h>
#include <ctype.h>

int main(void)
{
    char character;

    printf("Enter a character: ");
    scanf(" %c", &character);

    if (isalpha((unsigned char)character))
        printf("%c is an alphabet.\n", character);
    else
        printf("%c is not an alphabet.\n", character);

    return 0;
}
