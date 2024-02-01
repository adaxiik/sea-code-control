// https://mrlvsb.github.io/upr-skripta/c/promenne/promenne.html, 9. runnable code block

    #include <stdio.h>

    int main() {
        int a = 1;
        int b = a = 5;
        printf("%d\n", a);
        printf("%d\n", b);

        return 0;
    }
    