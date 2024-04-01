// https://mrlvsb.github.io/upr-skripta/c/prace_s_pameti/ukazatele_na_funkce.html, 2. runnable code block

#include <stdio.h>

int proved_pro_kladne(int cislo, int operace) {
    if (cislo <= 0) return cislo;
    
    if (operace == 0) {
        return cislo * 3;
    } else if (operace == 1) {
        return cislo + 1;
    } else {
        printf("Cislo: %d\n", cislo);
        return cislo;
    }
}

int main() {
    printf("%d\n", proved_pro_kladne(-1, 0));
    printf("%d\n", proved_pro_kladne(1, 0));
    printf("%d\n", proved_pro_kladne(1, 1));
    printf("%d\n", proved_pro_kladne(1, 2));

    return 0;
}