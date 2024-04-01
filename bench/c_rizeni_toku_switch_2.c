// https://mrlvsb.github.io/upr-skripta/c/rizeni_toku/switch.html, 2. runnable code block

#include <stdio.h>

int main() {
    switch (10) {
        case 0: printf("0");
        case 1: printf("1");
        case 5: printf("5");
        case 2: printf("2");
        default: printf("nenalezeno");
    }

    return 0;
}