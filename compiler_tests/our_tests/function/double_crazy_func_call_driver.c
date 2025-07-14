#include <stdio.h>

double foo(double x);

int main()
{
    printf("Example function returned: %f\n", foo(1.0));
    return !(foo(1.0) == 1.0);
}
