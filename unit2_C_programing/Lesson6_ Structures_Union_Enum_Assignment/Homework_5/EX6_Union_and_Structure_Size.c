#include <stdio.h>

union ExampleUnion {
    char bytes[32];
    int integer_value;
    double decimal_value;
};

struct ExampleStructure {
    char bytes[32];
    int first_value;
    int second_value;
};

int main(void)
{
    printf("size of union = %zu\n", sizeof(union ExampleUnion));
    printf("size of structure = %zu\n", sizeof(struct ExampleStructure));
    return 0;
}
