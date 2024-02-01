// https://mrlvsb.github.io/upr-skripta/c/promenne/promenne.html, 6. runnable code block

    #include <stdio.h>

    int main() {
        int a = 5;
        int b = a;
        a = 8;

        printf("%d\n", a);
        printf("%d\n", b);

        return 0;
    }
    