// https://mrlvsb.github.io/upr-skripta/c/text/retezce.html, 3. runnable code block

#include <stdio.h>

int main() {
    char text[] = "UPR";
    text[0] = 'A';
    printf("%s\n", text);
    return 0;
}