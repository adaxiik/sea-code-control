// https://mrlvsb.github.io/upr-skripta/c/rizeni_toku/for.html, 2. runnable code block

    #include <stdio.h>

    int main() {
        for (int a = 0; a <= 5; a = a + 1) {
           printf("iterace %d\n", a);
           if (a <= 2) {
             a = a + 1;
           }
        }

        return 0;
    }
    