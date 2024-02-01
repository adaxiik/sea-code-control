// https://mrlvsb.github.io/upr-skripta/c/prace_s_pameti/dynamicka_pamet.html, 2. runnable code block

#include <stdlib.h>

int* naalokuj_pamet() {
    int* pamet = (int*) malloc(sizeof(int));
    *pamet = 5;
    return pamet; 
}
int main() {
    int* pamet = naalokuj_pamet();
    printf("%d\n", *pamet);

    free(pamet); // uvolnění paměti, vysvětleno níže

    return 0;
}
