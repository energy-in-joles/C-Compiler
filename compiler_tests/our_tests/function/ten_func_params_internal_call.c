int g(int a, int b, int c, int d, int e, int f, int g, int h, int i, int j)
{
    return a+b+c+d+e+f+g+h+i+j;
}

int f()
{
    int a,b,c,d,e,x,y,h,i;
    a = 1;
    b = 2;
    c = 3;
    d = 4;
    e = 5;
    x = 6;
    y = 7;
    h = 8;
    i = 9;
    int z = g(a,b,c,d,e,x,y,h,i,10);
    return z;
}
