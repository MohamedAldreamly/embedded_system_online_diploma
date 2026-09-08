#include <stdio.h>

int main(void)
{
    int *ab;
    int m;
    m = 29;

    printf("\n\n Pointer : How to handle the pointers in the program :\n");
    printf("------------------------------------------------------------\n");
    printf(" Here in the declaration ab = int pointer, int m= 29\n\n");
    printf(" Address of m : %p\n", (void *)&m);
    printf(" Value of m : %d\n\n", m);

    ab = &m;
    printf(" Now ab is assigned with the address of m.\n");
    printf(" Address of pointer ab : %p\n", (void *)ab);
    printf(" Content of pointer ab : %d\n\n", *ab);

    m = 34;
    printf(" The value of m assigned to 34 now.\n");
    printf(" Address of pointer ab : %p\n", (void *)ab);
    printf(" Content of pointer ab : %d\n\n", *ab);

    *ab = 7;
    printf(" The pointer variable ab is assigned the value 7 now.\n");
    printf(" Address of m : %p\n", (void *)&m);
    printf(" Value of m : %d\n\n", m);

    return 0;
}
