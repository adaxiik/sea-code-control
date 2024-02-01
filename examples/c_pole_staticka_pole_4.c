// https://mrlvsb.github.io/upr-skripta/c/pole/staticka_pole.html, 4. runnable code block

#include <stdio.h>

void vypis_pole(int* pole) {
    printf("%d\n", pole[0]);
}

int main() {
    int pole[3] = { 1, 2, 3 };
    vypis_pole(pole);
    return 0;
}