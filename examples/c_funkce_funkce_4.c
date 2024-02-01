// https://mrlvsb.github.io/upr-skripta/c/funkce/funkce.html, 4. runnable code block

#include <stdio.h>

void vypis_cisla(int a, int b) {
    printf("Cislo a: %d\n", a);
    printf("Cislo b: %d\n", b);
}
int main() {
    vypis_cisla(5 + 5, 11 * 2);
    return 0;
}
