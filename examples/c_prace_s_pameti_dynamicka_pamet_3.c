// https://mrlvsb.github.io/upr-skripta/c/prace_s_pameti/dynamicka_pamet.html, 3. runnable code block

#include <stdlib.h>

int main() {
    int* p = (int*) malloc(sizeof(int)); // alokace paměti
    *p = 0;                              // použití paměti
    free(p);                             // uvolnění paměti

    return 0;
}
