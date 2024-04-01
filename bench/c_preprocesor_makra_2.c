// https://mrlvsb.github.io/upr-skripta/c/preprocesor/makra.html, 2. runnable code block

    #include <stdio.h>

    #define MOCNINA(a) a * a

    int main() {
        printf("%d\n", MOCNINA(1 + 1));
        return 0;
    }
    