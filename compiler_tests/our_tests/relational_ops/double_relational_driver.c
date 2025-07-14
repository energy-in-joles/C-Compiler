#include <stdio.h>

int f();

int g() {
    double a = 5.5, b = 2.3, c = 7.0;

    int result = (a + b > c) ? 1 : 0;
    result += (a - b < c) ? 2 : 0;
    result += (a * b == 12.65) ? 4 : 0;
    result += (a / b == 2.391304347826087) ? 8 : 0;
    result += (a++ == 5.5) ? 16 : 0;
    result += (++a == 7.5) ? 32 : 0;
    result += (a + b != 10.3) ? 64 : 0;
    result += (a * b == 17.25) ? 128 : 0;

    return result;
}

int main()
{
    printf("Hello from RISC-V\n");
    printf("Example function returned: %d\n", f());
    printf("Test function returned: %d\n", g());

    return !((f() == g()));
}



