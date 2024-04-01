// https://mrlvsb.github.io/upr-skripta/c/funkce/funkce.html, 14. runnable code block

    #include <stdio.h>

    int vypocet(int x) {
        if (x > 5) {
            return x + 1;
        }
        return x * 2;
    }

    int main() {
        int a = 6;
        int b = 4;
        int c = vypocet(vypocet(a) + vypocet(b));
        printf("%d\n", c);

        return 0;
    }
    