float f();

int main()
{
    printf("%f\n", f());
    return !(f() == 5.0f);
}
