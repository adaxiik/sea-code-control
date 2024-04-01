// https://mrlvsb.github.io/upr-skripta/c/pole/staticka_pole.html, 6. runnable code block

    #include <stdio.h>

    void prijmi_pole(int p[3]) {
        p[2] += 1;
    }

    int main() {
        int pole[3] = { 1, 2, 3 };

        prijmi_pole(pole);

        printf("{ %d, %d, %d }\n", pole[0], pole[1], pole[2]);

        return 0;
    }
