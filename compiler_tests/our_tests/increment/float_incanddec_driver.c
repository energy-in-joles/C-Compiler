#include <stdio.h>

float f();
float g();
float h();
float i();

int main()
{
    printf("Hello from RISC-V\n");
    return !(f() == 5.0f && g() == 4.0f && h() == 6.0f && i() == 5.0f);
}
