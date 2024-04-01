// https://mrlvsb.github.io/upr-skripta/c/pole/staticka_pole.html, 7. runnable code block

    #include <stdio.h>

    int main() {
        int pole[3] = { 1, 2, 3 };
        int *p = pole;

        p[1] = 5;
        pole[0] = 8;

        printf("%d, %d\n", *p, pole[1]);

        return 0;
    }
    