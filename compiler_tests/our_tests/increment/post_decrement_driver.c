#include <stdio.h>

int f();
int g();

int main()
{
    printf("Hello from RISC-V\n");
    printf("f() = %d\n", f());
    printf("g() = %d\n", g());
    return !(f() == 5 && g() == 4);
}
