float f()
{
    float x = 5.0f;
    float y = 2.0f;
    x = y + 5.0f + x;
    return x;
}

float g()
{
    float x = 5.0f;
    float y = 3.0f;
    x *= ((y - 5.0f) / x);
    return x;
}
