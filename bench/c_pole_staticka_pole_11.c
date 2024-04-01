// https://mrlvsb.github.io/upr-skripta/c/pole/staticka_pole.html, 11. runnable code block

    #include <stdio.h>

    int main() {
        char pole[3];
        char* ptr = pole;

        printf("%d\n", (int) sizeof(pole));
        printf("%d\n", (int) sizeof(ptr));

        return 0;
    }
    