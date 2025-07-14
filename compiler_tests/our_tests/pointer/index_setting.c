int f(int **p)
{
    (*p)[1] = 5;
    return (*p)[1];
}
