#include <stdio.h>

struct employee
{
    char *empname;
    int empid;
};

int main(void)
{
    static struct employee emp1 = {"Jhon", 1001};
    static struct employee emp2 = {"Alex", 1002};
    static struct employee emp3 = {"Taylor", 1003};
    struct employee *arr[] = {&emp1, &emp2, &emp3};
    struct employee *(*pt)[3] = &arr;

    printf("\n\n Pointer : Show a pointer to an array which contents are pointer to structure :\n");
    printf("-------------------------------------------------------------------------------\n");
    printf(" Employee Name : %s \n", (**(*pt + 1)).empname);
    printf(" Employee ID : %d\n", (*(*pt + 1))->empid);
    printf("\n\n");

    return 0;
}
