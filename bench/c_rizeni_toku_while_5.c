// https://mrlvsb.github.io/upr-skripta/c/rizeni_toku/while.html, 5. runnable code block

#include <stdio.h>

int main() {
    int pocet = 0;
    int pocet2 = 1;
    while (1) {
        printf("Hodnota: %d\n", pocet);
        pocet = pocet + 1;
        pocet2 += pocet * 2;

        if (pocet > 10) break;
        if (pocet2 > 64) break;
    }

    return 0;
}