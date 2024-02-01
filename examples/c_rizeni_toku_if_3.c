// https://mrlvsb.github.io/upr-skripta/c/rizeni_toku/if.html, 3. runnable code block

#include <stdio.h>

int main() {
    int delka_hesla = 4;
    int delka_jmena = 3;
    if (delka_hesla > 5) {
        if (delka_jmena > 3) {
            printf("Uzivatel byl zaregistrovan\n");
        } else {
            printf("Uzivatelske jmeno neni dostatecne dlouhe\n");
        }
    } else {
        printf("Heslo neni dostatecne dlouhe\n");
    }

    return 0;
}
