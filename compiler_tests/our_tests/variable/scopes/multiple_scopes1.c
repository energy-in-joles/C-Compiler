int f()
{
    int x;
    x=11;
    {
        int x;
        x=22;
        {
            int x;
            x=33;
            {
                int x;
                x=44;
            }
        }
    }
    return x;
}
