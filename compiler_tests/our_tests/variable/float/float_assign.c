float f()
{
    float x = 5.0f;
    float y = 2.0f;
    x = y;
    return x;
}

float g()
{
    float x = 5.0f;
    x = 3.0f;
    return x;
}

float y = 5.0f;
float h()
{
    float x = 5.0f;
    x += y;
    y += x;
    return y;
}
