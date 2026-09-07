#include <stdio.h>
#include <ctype.h>

int main(void)
{
    char alphabet;
    char lower_case;

    printf("Enter an alphabet: ");
    scanf(" %c", &alphabet);
    lower_case = (char)tolower((unsigned char)alphabet);

    if (lower_case == 'a' || lower_case == 'e' || lower_case == 'i' ||
        lower_case == 'o' || lower_case == 'u')
        printf("%c is a vowel.\n", alphabet);
    else
        printf("%c is a consonant.\n", alphabet);

    return 0;
}
