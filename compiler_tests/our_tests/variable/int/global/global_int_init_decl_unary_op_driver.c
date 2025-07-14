#include <stdio.h>

int f();
int g();
int h();
int i();

int main()
{
    printf("Hello from RISC-V\n");

    return !(f() == 5 && i() == 5 && g() == -6 && h() == 0);
}
