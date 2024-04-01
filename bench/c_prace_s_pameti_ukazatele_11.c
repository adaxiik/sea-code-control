// https://mrlvsb.github.io/upr-skripta/c/prace_s_pameti/ukazatele.html, 11. runnable code block

    #include <stdio.h>

    int main() {
        int a = 2;
        int b = 3;

        int* p = &a;
        int** px = &p;
        *px = &b;

        *p = 8;

        printf("a = %d, b = %d\n", a, b);

        return 0;
    }
    