// https://mrlvsb.github.io/upr-skripta/c/rizeni_toku/ternarni_operator.html, 1. runnable code block

#include <stdio.h>

int main() {
    int a = 1;
    int b = 5;
    int c = (a >= b) ? a - b : a + b;
    printf("%d\n", c);

    return 0;
}