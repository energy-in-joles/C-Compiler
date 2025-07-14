#include <stdio.h>

int g();

int main()
{
    printf("Hello from RISC-V\n");
    printf("Example function returned: %d\n", g());

    return !(g() == 987);
}
