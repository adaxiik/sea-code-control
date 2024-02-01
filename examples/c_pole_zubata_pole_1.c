// https://mrlvsb.github.io/upr-skripta/c/pole/zubata_pole.html, 1. runnable code block

#include <stdlib.h>

int main() {
    // Vytvoření pole studentů
    int** studenti = (int**) malloc(5 * sizeof(int*));

    for (int i = 0; i < 5; i++) {
        // Vytvoření pole předmětů pro konkrétního studenta
        studenti[i] = (int*) malloc((i + 1) * sizeof(int));
    }

    // Druhý předmět třetího studenta bude mít ID 5
    studenti[2][1] = 5;

    for (int i = 0; i < 5; i++) {
        // Uvolnění pole předmětů pro konkrétního studenta
        free(studenti[i]);
    }

    // Uvolnění pole studentů
    free(studenti);
    return 0;
}
