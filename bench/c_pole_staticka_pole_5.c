// https://mrlvsb.github.io/upr-skripta/c/pole/staticka_pole.html, 5. runnable code block

    #include <stdio.h>

    int main() {
        int pole[3] = { 1, 4, 7 };
        int a = *pole + 1;
        int b = *(pole + 1);

        printf("a = %d, b = %d\n", a, b);

        return 0;
    }
    