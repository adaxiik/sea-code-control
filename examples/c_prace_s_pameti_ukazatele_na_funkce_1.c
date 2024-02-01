// https://mrlvsb.github.io/upr-skripta/c/prace_s_pameti/ukazatele_na_funkce.html, 1. runnable code block

#include <stdio.h>

int funkce(int x) {
    printf("Funkce zavolana s parametrem %d\n", x);
    return x + 1;
}

int main() {
    int (*ukazatel)(int) = funkce;
    
    int ret = ukazatel(1);
    printf("Funkce vratila %d\n", ret);

    return 0;
}