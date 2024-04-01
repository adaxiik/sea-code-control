// https://mrlvsb.github.io/upr-skripta/c/pole/staticka_pole.html, 12. runnable code block

    #include <stdio.h>

    void print_size(char pole[3]) {
        printf("%d\n", (int) sizeof(pole));
    }
   
    int main() {
        char pole[3];
        print_size(pole);

        return 0;
    }
    