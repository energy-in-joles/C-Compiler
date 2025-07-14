int f();
float g();

int main()
{
    printf("%d\n", f());
    printf("%f\n", g());
    return !(f() == 1 && g() == 10.0f);
}
