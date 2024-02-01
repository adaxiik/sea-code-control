// https://mrlvsb.github.io/upr-skripta/c/struktury/struktury_funkce.html, 2. runnable code block

    #include <stdio.h>

    typedef struct {
        int vek;
    } Osoba;
   
    void oslav_narozeniny(Osoba osoba) {
        osoba.vek += 1;
    }

    int main() {
        Osoba milan = { .vek = 17 };
        oslav_narozeniny(milan);
        printf("Vek Milana: %d\n", milan.vek);

        return 0;
    }
    