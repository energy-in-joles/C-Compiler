int f() {
    int x = 0;
    int i;
    for (i = 0; i < 10; i++) {
        if (i % 2 == 0) {
            continue;
        }
        x += i;
    }
    return x;
}
