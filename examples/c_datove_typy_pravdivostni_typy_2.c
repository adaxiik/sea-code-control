// https://mrlvsb.github.io/upr-skripta/c/datove_typy/pravdivostni_typy.html, 2. runnable code block

#include <stdbool.h>
#include <stdio.h>

int main() {
    int delka_hesla = 8;
    bool email_overen = false;
    int rok_narozeni = 1994;

    bool uzivatel_validni = delka_hesla >= 9 && (email_overen || rok_narozeni > 1990); // false
    bool uzivatel_validni2 = delka_hesla >= 9 && email_overen || rok_narozeni > 1990; // true

    printf("%d\n", uzivatel_validni);
    printf("%d\n", uzivatel_validni2);

    return 0;
}
