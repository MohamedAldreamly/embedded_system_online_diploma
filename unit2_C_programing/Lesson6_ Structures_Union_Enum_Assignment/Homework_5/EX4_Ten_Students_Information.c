#include <stdio.h>

#define STUDENT_COUNT 10

struct Student {
    char name[50];
    int roll;
    float marks;
};

int main(void)
{
    struct Student students[STUDENT_COUNT];
    int index;

    printf("Enter information of students:\n");
    for (index = 0; index < STUDENT_COUNT; ++index) {
        students[index].roll = index + 1;
        printf("\nFor roll number %d\n", students[index].roll);
        printf("Enter name: ");
        scanf("%49s", students[index].name);
        printf("Enter marks: ");
        scanf("%f", &students[index].marks);
    }

    printf("\nDisplaying information of students:\n");
    for (index = 0; index < STUDENT_COUNT; ++index) {
        printf("\nInformation for roll number %d:\n", students[index].roll);
        printf("Name: %s\n", students[index].name);
        printf("Marks: %.1f\n", students[index].marks);
    }
    return 0;
}
