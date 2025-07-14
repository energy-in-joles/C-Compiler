float f();
float g();
float h();

int main()
{
    printf("%f\n", f());
    return !(f() == 2.0f && g() == 3.0f && h() == 15.0f);
}
