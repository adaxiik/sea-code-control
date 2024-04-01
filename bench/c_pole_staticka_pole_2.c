// https://mrlvsb.github.io/upr-skripta/c/pole/staticka_pole.html, 2. runnable code block

#include <stdio.h>

int main() {
    int pole[3] = { 1, 2, 3 };
    printf("%d\n", *(pole + 0));   // první prvek pole
    printf("%d\n", *(pole + 1));   // druhý prvek pole
    printf("%d\n", *(pole + 2));   // třetí prvek pole
    return 0;
}