#include <stdio.h>

struct Distance {
    int feet;
    float inch;
};

int main(void)
{
    struct Distance first, second, sum;

    printf("Enter information for 1st distance\n");
    printf("Enter feet: ");
    scanf("%d", &first.feet);
    printf("Enter inch: ");
    scanf("%f", &first.inch);

    printf("\nEnter information for 2nd distance\n");
    printf("Enter feet: ");
    scanf("%d", &second.feet);
    printf("Enter inch: ");
    scanf("%f", &second.inch);

    sum.feet = first.feet + second.feet;
    sum.inch = first.inch + second.inch;
    while (sum.inch >= 12.0f) {
        sum.inch -= 12.0f;
        ++sum.feet;
    }

    printf("\nSum of distances = %d'-%.1f\"\n", sum.feet, sum.inch);
    return 0;
}
