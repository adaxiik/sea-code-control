// https://mrlvsb.github.io/upr-skripta/c/prace_s_pameti/ukazatele.html, 6. runnable code block

    #include <stdio.h>

    int main() {
        int a = 2;
        int* p = &a;
        p = 5;

        printf("%d\n", a);

        return 0;
    }
    