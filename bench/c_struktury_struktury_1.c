// https://mrlvsb.github.io/upr-skripta/c/struktury/struktury.html, 1. runnable code block

#include <stdio.h>

struct Osoba {
    int vek;
    int pocet_pratel;
};

int main() {
    struct Osoba martina = { .vek = 18, .pocet_pratel = 10 };
    martina.vek += 1;           // přístup k členu `vek`
    martina.pocet_pratel += 20; // přístup k členu `pocet_pratel`
    printf("Martina ma %d let a ma %d pratel\n", martina.vek, martina.pocet_pratel);

    return 0;
}