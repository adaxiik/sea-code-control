// https://mrlvsb.github.io/upr-skripta/c/rizeni_toku/while.html, 6. runnable code block

#include <stdio.h>

int main() {
    int i = 0;
    while (i < 3) {
        // printf("i: %d\n", i);
        int j = 0;
        while (j < 4) {
            // printf("  j: %d\n", j);
            j = j + 1;
        }

        i = i + 1;
    }
    printf("Konec programu\n");

    return 0;
}
