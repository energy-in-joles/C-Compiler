#include <stdio.h>

int f();

int main()
{
    printf("Hello from RISC-V\n");

    return !((f(1, 0) == 1) && (f(1, 1) == 1) && (f(0, 0) == 0) && (f(0, 1) == 1));
}
