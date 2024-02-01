// https://mrlvsb.github.io/upr-skripta/c/prace_s_pameti/ukazatele.html, 1. runnable code block

    #include <stdio.h>

    int main() {
        int x = 1;
        int* p = &x;

        printf("%d\n", x);  // hodnota proměnné x
        printf("%p\n", p);  // adresa v paměti, kde je uložena proměnná x

        return 0;
    }
    