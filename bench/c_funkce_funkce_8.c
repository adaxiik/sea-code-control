// https://mrlvsb.github.io/upr-skripta/c/funkce/funkce.html, 8. runnable code block

    #include <stdio.h>

    void zmen_cislo(int cislo) {
        cislo = 5;
    }

    int main() {
        int cislo = 8;
        zmen_cislo(cislo);
        printf("%d\n", cislo);

        return 0;
    }
    