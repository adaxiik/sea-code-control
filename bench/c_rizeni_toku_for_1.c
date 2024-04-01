// https://mrlvsb.github.io/upr-skripta/c/rizeni_toku/for.html, 1. runnable code block

    #include <stdio.h>

    int main() {
        int a = 5;
        for (; a >= 0; a = a - 1) {
           printf("iterace %d\n", a);
        }
        printf("a = %d\n", a);

        return 0;
    }
    