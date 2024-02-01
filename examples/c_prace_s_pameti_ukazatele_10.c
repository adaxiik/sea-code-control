// https://mrlvsb.github.io/upr-skripta/c/prace_s_pameti/ukazatele.html, 10. runnable code block

    #include <stdio.h>

    int main() {
        int a = 2;
        int* p = &a;

        printf("%d\n", p);

        return 0;
    }
    