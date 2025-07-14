#include <stdio.h>

float bar(float f1, float f2, float f3, float f4, float f5, float f6, float f7, float f8, float f9);

int main()
{
    printf("Example function returned: %f\n", bar(1.0f,2.0f,3.0f,4.0f,5.0f,6.0f,7.0f,8.0f,9.0f));
    return !(bar(1.0f,2.0f,3.0f,4.0f,5.0f,6.0f,7.0f,8.0f,9.0f) == 9.0f);
}
