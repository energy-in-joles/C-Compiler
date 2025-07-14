int f() {
    int x = 0;
    int sum = 0;
    while (x < 10) {
        x++;
        if (x % 2 == 0) {
            continue;
        }
        sum += x;
    }
    return sum;
}
