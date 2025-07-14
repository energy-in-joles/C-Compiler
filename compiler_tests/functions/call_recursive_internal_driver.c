#include <stdio.h>

int f(int n);

int main()
{
    printf("%d\n", f(10));
    return !(f(5)==15);
}
