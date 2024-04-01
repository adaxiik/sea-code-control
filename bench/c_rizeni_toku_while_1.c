// https://mrlvsb.github.io/upr-skripta/c/rizeni_toku/while.html, 1. runnable code block

#include <stdio.h>

int main() {
    int pocet = 0;
    while (pocet < 5) {
        printf("Telo cyklu se provedlo, hodnota promenne pocet=%d\n", pocet);
        pocet = pocet + 1;
    }
    return 0;
}