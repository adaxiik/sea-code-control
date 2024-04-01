// https://mrlvsb.github.io/upr-skripta/c/funkce/funkce.html, 12. runnable code block

    #include <stdio.h>

    int umocni(int x) {
        return x * x;
    }

    int main() {
        int cislo = 5;
        umocni(cislo);
        printf("%d\n", cislo);

        return 0;
    }
    