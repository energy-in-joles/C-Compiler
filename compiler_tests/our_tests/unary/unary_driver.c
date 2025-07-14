#include <stdio.h>

int f();
int g();
int h();
int i();
int main()
{
    printf("Hello from RISC-V\n");
    return !(f() == -5 && g() == 5 && h() == 1 && i() == -6);
}
