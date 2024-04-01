// https://mrlvsb.github.io/upr-skripta/c/text/retezce.html, 6. runnable code block

    #include <stdio.h>

    int main() {
        const char* str = "hello";
        
        for (int i = 0; i < 5; i++) {
            printf("%c", str[i] - 32);
        }
        printf("\n");

        return 0;
    }
    