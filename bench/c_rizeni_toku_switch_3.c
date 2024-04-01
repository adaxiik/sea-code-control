// https://mrlvsb.github.io/upr-skripta/c/rizeni_toku/switch.html, 3. runnable code block

#include <stdio.h>

int main() {
    switch (1) {
        case 0: printf("0"); break;
        case 1: printf("1"); break;
        case 2: printf("2"); break;
        default: printf("nenalezeno");
    }

    return 0;
}