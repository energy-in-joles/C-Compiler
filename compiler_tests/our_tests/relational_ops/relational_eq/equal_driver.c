#include <stdio.h>

int f();
int g();

int main()
{
    printf("Hello from RISC-V\n");
    printf("Example function returned: %d\n", f());

    return !((f() & !g()) == 1);
}
