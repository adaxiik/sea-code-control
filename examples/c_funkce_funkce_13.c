// https://mrlvsb.github.io/upr-skripta/c/funkce/funkce.html, 13. runnable code block

    #include <stdio.h>

    void vypis_cislo(int x) {
        if (x < 0) {
            return;
        }
        printf("cislo = %d\n", x);
    }

    int main() {
        vypis_cislo(1);
        vypis_cislo(-1);

        return 0;
    }
    