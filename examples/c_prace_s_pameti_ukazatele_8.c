// https://mrlvsb.github.io/upr-skripta/c/prace_s_pameti/ukazatele.html, 8. runnable code block

    #include <stdio.h>

    void zmen_ukazatel(int* p, int a) {
        p = &a;
    }

    int main() {
        int a = 2;
        int b = 3;

        int* p = &b;
        zmen_ukazatel(p, a);

        printf("%d\n", *p);

        return 0;
    }
    