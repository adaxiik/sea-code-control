// https://mrlvsb.github.io/upr-skripta/c/struktury/struktury_funkce.html, 3. runnable code block

    #include <stdio.h>

    typedef struct {
        char* jmeno;
    } Osoba;

    void uprav_jmeno(Osoba osoba) {
        osoba.jmeno[0] = 'k';
    }

    int main() {
        char jmeno[] = "Karel";

        Osoba karel = { .jmeno = jmeno };
        uprav_jmeno(karel);
        printf("Jmeno Karla: %s\n", karel.jmeno);

        return 0;
    }
    