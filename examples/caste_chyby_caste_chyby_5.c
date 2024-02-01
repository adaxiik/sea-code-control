// https://mrlvsb.github.io/upr-skripta/caste_chyby/caste_chyby.html, 5. runnable code block

#include <stdio.h>

int main() {
    if (0) { /* zde není co provést */ }

    // tento blok se provede vždy
    {
      printf("Hello\n");
    }

    return 0;
}
