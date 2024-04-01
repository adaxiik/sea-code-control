// https://mrlvsb.github.io/upr-skripta/c/rizeni_toku/while.html, 3. runnable code block

    #include <stdio.h>
    
    int main() {
        int pocet = 0;
        while (pocet < 10) {
            pocet = pocet + 1;

            if (pocet < 5) {
                continue;
            }

            printf("Hodnota: %d\n", pocet); 
        }
    
        return 0;
    }
    