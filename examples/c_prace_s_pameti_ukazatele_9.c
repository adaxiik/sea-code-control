// https://mrlvsb.github.io/upr-skripta/c/prace_s_pameti/ukazatele.html, 9. runnable code block

    #include <stdio.h>

    int main() {
        int a = 2;

        int* p = &a;
        *p = 4;

        int b = *p;
        *p = 8;

        printf("a = %d, b = %d\n", a, b);

        return 0;
    }
    