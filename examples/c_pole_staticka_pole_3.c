// https://mrlvsb.github.io/upr-skripta/c/pole/staticka_pole.html, 3. runnable code block

#include <stdio.h>

int main() {
    int pole[10] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    for (int i = 0; i < 10; i++) {
        printf("%d ", pole[i]);
    }
    return 0;
}