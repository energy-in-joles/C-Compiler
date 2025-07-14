int f()
{
    int x = 5;
    int *y = &x;
    int **z = &y;
    return **z;
}
