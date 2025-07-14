float f();
float g();

int main()
{
    printf("%f\n", f());
    printf("%f\n", g());
    return !(f() == 12.0f && g() == -2.0f);
}
