// https://mrlvsb.github.io/upr-skripta/c/prace_s_pameti/ukazatele.html, 3. runnable code block

#include <stdio.h>

int main() {
    int cislo = 1;
    int* ukazatel = &cislo;
    *ukazatel = 5;

    printf("%d\n", cislo);

    return 0;
}