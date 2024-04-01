// https://mrlvsb.github.io/upr-skripta/c/prace_s_pameti/dynamicka_pamet.html, 1. runnable code block

#include <stdio.h>
int main() {
    printf("Velikost int je: %lu\n", sizeof(int));
    printf("Velikost int* je: %lu\n", sizeof(int*));
    return 0;
}