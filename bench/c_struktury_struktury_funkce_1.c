// https://mrlvsb.github.io/upr-skripta/c/struktury/struktury_funkce.html, 1. runnable code block

#include <stdbool.h>
#include <stdio.h>

typedef struct {
    float x;
    float y;
} Poloha;

typedef struct {
    const char* jmeno;
    int skore;
    Poloha poloha;
} Hrac;

void hrac_posun(Hrac* hrac, int x, int y) {
    hrac->poloha.x += x;
    hrac->poloha.y += y;
}
void hrac_pridej_skore(Hrac* hrac, int skore) {
    hrac->skore += skore;
    if (hrac->skore > 100) {
        hrac->skore = 100;
    }
}
bool hrac_vyhral(Hrac* hrac) {
    return hrac->skore == 100;
}

int main() {
    Hrac hrac = { .jmeno = "Jindrich", .skore = 40, .poloha = { .x = 10, .y = 20 } };
    hrac_posun(&hrac, 5, -8);
    hrac_pridej_skore(&hrac, 70);

    printf("Hrac vyhral: %d\n", hrac_vyhral(&hrac));

    return 0;
}