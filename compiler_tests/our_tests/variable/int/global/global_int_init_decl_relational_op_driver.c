#include <stdio.h>

int f();
int g();
int h();
int i();
int j();
int k();

int main()
{
    printf("Hello from RISC-V\n");

    return !(f() == 0 && g() == 1 && h() == 0 && i() == 1 && j() == 1 && k() == 0);
}
