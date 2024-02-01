// https://mrlvsb.github.io/upr-skripta/caste_chyby/caste_chyby.html, 2. runnable code block

#include <stdio.h>

int main() {
    int a = 100;

    // špatně
    if (2 < a < 8) {
        printf("A: a patri do intervalu (2, 8)\n");
    }

    // správně
    if (2 < a && a < 8) {
        printf("B: a patri do intervalu (2, 8)\n");
    }

    return 0;
}
