// https://mrlvsb.github.io/upr-skripta/c/rizeni_toku/if.html, 1. runnable code block

#include <stdio.h>

int main() {
    int delka_hesla = 5;

    printf("Kontroluji heslo...\n");
    if (delka_hesla > 5) {
        printf("Heslo je dostatecne dlouhe\n");
    }
    printf("Kontrola hesla dokoncena\n");

    return 0;
}