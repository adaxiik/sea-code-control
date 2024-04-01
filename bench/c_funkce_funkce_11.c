// https://mrlvsb.github.io/upr-skripta/c/funkce/funkce.html, 11. runnable code block

    #include <stdio.h>

    int vrat_cislo(int x) {
        return x;
    }

    int main() {
        int cislo = 5;
        vrat_cislo(cislo) = 8;
        printf("%d\n", cislo);

        return 0;
    }
    