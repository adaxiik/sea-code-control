// https://mrlvsb.github.io/upr-skripta/c/datove_typy/pravdivostni_typy.html, 3. runnable code block

    #include <stdio.h>
    #include <stdbool.h>

    int main() {
        int pocet_zidli = 14;
        int pocet_lidi = 8;
        int pocet_znicenych_zidli = 4;

        bool dostatek_zidli = (pocet_zidli - pocet_znicenych_zidli) >= pocet_lidi;
        bool dostatek_lidi = pocet_lidi >= 6;
        bool party_pripravena = dostatek_zidli && dostatek_lidi;

        printf("Party: %d\n", party_pripravena);

        return 0;
    }
    