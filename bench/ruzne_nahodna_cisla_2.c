// https://mrlvsb.github.io/upr-skripta/ruzne/nahodna_cisla.html, 2. runnable code block

#include <stdlib.h>
#include <time.h>

int main() {
    int cas = (int) time(NULL); // získej současný čas
    srand(cas); // nastav S na současný čas

    int cislo1 = rand(); // pseudo-náhodné číslo z intervalu [0, RAND_MAX]
    int cislo2 = rand() % 100; // z intervalu [0, 99]
    int cislo3 = rand() % 100 + 5; // z intervalu [5, 104]
    float cislo4 = rand() / (float) RAND_MAX; // z intervalu [0.0, 1.0]

    return 0;
}