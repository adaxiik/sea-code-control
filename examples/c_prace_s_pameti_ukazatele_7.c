// https://mrlvsb.github.io/upr-skripta/c/prace_s_pameti/ukazatele.html, 7. runnable code block

    #include <stdio.h>

    int main() {
        int a = 2;
        int b = 3;

        int* p = &a;
        p = &b;

        *p += 1;

        printf("a = %d, b = %d\n", a, b);

        return 0;
    }
    