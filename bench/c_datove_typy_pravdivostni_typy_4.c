// https://mrlvsb.github.io/upr-skripta/c/datove_typy/pravdivostni_typy.html, 4. runnable code block

    #include <stdio.h>
    #include <stdbool.h>

    int main() {
        int a = 5;
        int b = 4;

        bool x = a >= 3 || (b = 8);

        printf("a=%d\n", a);
        printf("b=%d\n", b);
        printf("x=%d\n", x);

        return 0;
    }
    