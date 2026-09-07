#include <stdio.h>

#define PI 3.1415f
#define AREA(radius) (PI * (radius) * (radius))

int main(void)
{
    int radius;
    float circle_area;

    printf("Enter the radius: ");
    scanf("%d", &radius);
    circle_area = AREA(radius);
    printf("Area = %.2f\n", circle_area);
    return 0;
}
