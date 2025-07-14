#include <stdio.h>

double f();
double g();
double h();
double i();

int main()
{
    printf("Hello from RISC-V\n");
    return !(f() == 5.0 && g() == 4.0 && h() == 6.0 && i() == 5.0);
}
