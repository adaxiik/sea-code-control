// https://mrlvsb.github.io/upr-skripta/c/funkce/funkce.html, 3. runnable code block

#include <stdio.h>

void vypis_cislo(int cislo) {
    printf("Cislo: %d\n", cislo);
    if (cislo < 0) {
        printf("Predane cislo je zaporne\n");
    } else {
        printf("Predane cislo je nezaporne\n");
    }
}
int main() {
    vypis_cislo(5);
    vypis_cislo(1 + 8);

    int x = -10;
    vypis_cislo(x);

    return 0;
}