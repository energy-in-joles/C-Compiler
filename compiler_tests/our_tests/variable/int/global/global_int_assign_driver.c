#include <stdio.h>

int f();
int g();

int main()
{
    printf("Hello from RISC-V\n");

    return !(f() == 5 && g() == -5);
}
