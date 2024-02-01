// https://mrlvsb.github.io/upr-skripta/c/funkce/funkce.html, 15. runnable code block

    #include <stdio.h>

    int cislo = 1;

    void uprav_promennou() {
        cislo = 2;
    }

    int main() {
        printf("%d\n", cislo);
        uprav_promennou();
        printf("%d\n", cislo);

        return 0;
    }
    