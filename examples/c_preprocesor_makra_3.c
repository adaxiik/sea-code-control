// https://mrlvsb.github.io/upr-skripta/c/preprocesor/makra.html, 3. runnable code block

    #include <stdio.h>

    #define MOCNINA(a) a * a

    int main() {
        int x = 0;
        int mocnina = MOCNINA(x++);
        printf("x=%d, mocnina=%d\n", x, mocnina);
  
        return 0;
    }
    