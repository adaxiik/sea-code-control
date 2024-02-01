// https://mrlvsb.github.io/upr-skripta/c/promenne/promenne.html, 1. runnable code block

#include <stdio.h>

int main() {
    int a = 5;
    int b = a;  // hodnota `b` je 5
    int c = b + a + 1;  // hodnota `c` je 11

    printf("a = %d, b krat 2 = %d, c = %d", a, b * 2, c);

    return 0;
}
