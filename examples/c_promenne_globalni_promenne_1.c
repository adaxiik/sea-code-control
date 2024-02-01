// https://mrlvsb.github.io/upr-skripta/c/promenne/globalni_promenne.html, 1. runnable code block

#include <stdio.h>

// zde nelze použít proměnnou `globalni_promenna`

int globalni_promenna = 1;

// zde lze použít proměnnou `globalni_promenna`

int main() {
    // zde lze použít proměnnou `globalni_promenna`

    globalni_promenna += 1;
    printf("%d\n", globalni_promenna);

    return 0;
}

void funkce2() {
    // zde lze použít proměnnou `globalni_promenna`
    printf("%d\n", globalni_promenna);
}
