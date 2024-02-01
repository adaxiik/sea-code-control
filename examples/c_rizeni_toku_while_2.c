// https://mrlvsb.github.io/upr-skripta/c/rizeni_toku/while.html, 2. runnable code block

#include <stdio.h>

int main() {
    int i = 0;
    while (i < 5) {
        if (i == 0) {
            printf("Prvni iterace\n");
        }
        printf("Hodnota i=%d\n", i);
        i += 1;
    }
    return 0;
}
