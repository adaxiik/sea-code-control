// https://mrlvsb.github.io/upr-skripta/c/funkce/funkce.html, 6. runnable code block

#include <stdio.h>

int treti_mocnina(int cislo) {
    return cislo * cislo * cislo;
}
int main() {
    printf("%d\n", treti_mocnina(5 + 1));
    return 0;
}