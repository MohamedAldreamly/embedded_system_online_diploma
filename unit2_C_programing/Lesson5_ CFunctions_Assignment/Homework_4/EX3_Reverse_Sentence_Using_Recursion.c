#include <stdio.h>

void reverse_sentence(void)
{
    int character = getchar();
    if (character != '\n' && character != EOF) {
        reverse_sentence();
        putchar(character);
    }
}

int main(void)
{
    printf("Enter a sentence: ");
    reverse_sentence();
    printf("\n");
    return 0;
}
