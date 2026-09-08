#include <stdio.h>

int main(void)
{
    char alph[27];
    int x;
    char *ptr;

    printf("\n\n Pointer : Print all the alphabets:\n");
    printf("--------------------------------------\n");

    ptr = alph;
    x = 0;
    while (x < 26)
    {
        *ptr = (char)(x + 'A');
        ptr++;
        x++;
    }

    ptr = alph;
    printf(" The Alphabets are: \n");
    x = 0;
    while (x < 26)
    {
        printf(" %c ", *ptr);
        ptr++;
        x++;
    }

    printf("\n\n");
    return 0;
}
