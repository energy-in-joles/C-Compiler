int f(int **y);

int main()
{
    int x[2];
    int *p = x;
    int **y = &p;
    x[1] = 15;

    return !(f(y) == 5);
}
