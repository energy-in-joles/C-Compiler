float f()
{
    float x;
    float *y=&x;
    *y=64.1f;
    return x;
}
