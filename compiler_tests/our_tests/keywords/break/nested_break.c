int f() {
    int x = 0;
    int i, j;
    for (i = 0; i < 5; i++) {
        for (j = 0; j < 5; j++) {
            if (j == 2) {
                break;
            }
            x++;
        }
    }
    return x;
}
