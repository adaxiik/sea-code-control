// https://mrlvsb.github.io/upr-skripta/c/rizeni_toku/while.html, 8. runnable code block

    #include <stdio.h>
    #include <stdbool.h>

    int main() {
        int a = 0;
        int b = 8;

        while (true) {
          if (a > 2) {
            printf("Hodnota a = %d\n", a);
          }

          if (a >= b) {
            break;
          }
          a = a + 2;
        }

        return 0;
    }
    