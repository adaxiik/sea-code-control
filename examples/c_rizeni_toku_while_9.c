// https://mrlvsb.github.io/upr-skripta/c/rizeni_toku/while.html, 9. runnable code block

    #include <stdio.h>

    int main() {
        int a = 0;
        int b = 6;

        while (a > b) {
          printf("Hodnota b = %d\n", b);
          b = b + 1;
        }

        return 0;
    }
    