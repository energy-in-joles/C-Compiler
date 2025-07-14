#include <stdio.h>

char f();
char g();

int main()
{
    printf("Example function returned: %d\n", f());
    printf("Example function returned: %d\n", g());
    return !(f() == 'i' && g() == 'o');
}
