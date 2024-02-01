// https://mrlvsb.github.io/upr-skripta/c/prace_s_pameti/ukazatele.html, 2. runnable code block

#include <stdio.h>

int main() {
    int cislo = 1;
    int* ukazatel = &cislo;

    printf("%p\n", ukazatel);
    printf("%d\n", *ukazatel);
    printf("%d\n", cislo);

    return 0;
}
