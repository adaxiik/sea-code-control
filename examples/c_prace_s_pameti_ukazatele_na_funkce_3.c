// https://mrlvsb.github.io/upr-skripta/c/prace_s_pameti/ukazatele_na_funkce.html, 3. runnable code block

#include <stdio.h>

int proved_pro_kladne(int cislo, int(*operace)(int)) {
    if (cislo <= 0) return cislo;
    return operace(cislo);
}

int vynasob_dvema(int cislo) { return cislo * 2; }
int pricti_jednicku(int cislo) { return cislo + 1; }
int vypis(int cislo) {
    printf("Cislo: %d\n", cislo);
    return cislo;
}

int main() {
    printf("%d\n", proved_pro_kladne(-1, vynasob_dvema));
    printf("%d\n", proved_pro_kladne(1, vynasob_dvema));
    printf("%d\n", proved_pro_kladne(1, pricti_jednicku));
    printf("%d\n", proved_pro_kladne(1, vypis));

    return 0;
}
