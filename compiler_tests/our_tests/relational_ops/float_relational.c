int f() {
    float a = 5.5f, b = 2.3f, c = 7.0f;

    int result = (a + b > c) ? 1 : 0;
    result += (a - b < c) ? 2 : 0;
    result += (a * b == 12.65f) ? 4 : 0;
    result += (a / b == 2.391304347826087f) ? 8 : 0;
    result += (a++ == 5.5f) ? 16 : 0;
    result += (++a == 7.5f) ? 32 : 0;
    result += (a + b != 10.3f) ? 64 : 0;
    result += (a * b == 17.25f) ? 128 : 0;

    return result;
}


