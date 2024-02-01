// https://mrlvsb.github.io/upr-skripta/c/rizeni_toku/while.html, 4. runnable code block

    #include <stdio.h>
    
    int main() {
        int pocet = 0;
        while (pocet < 10) {
            if (pocet * 2 > 12) {
                break;
            }

            printf("Hodnota: %d\n", pocet);
            pocet = pocet + 1;
        }
    
        return 0;
    }
    