// https://mrlvsb.github.io/upr-skripta/c/funkce/funkce.html, 9. runnable code block

    #include <stdio.h>

    void vytvor_promennou() {
        int x = 5;
    }

    int main() {
        vytvor_promennou();
        printf("%d\n", x);

        return 0;
    }
    