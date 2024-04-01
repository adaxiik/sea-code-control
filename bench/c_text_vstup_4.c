// https://mrlvsb.github.io/upr-skripta/c/text/vstup.html, 4. runnable code block

    #include <stdio.h>

    int main() {
        char radek[100];
        fgets(radek, sizeof(radek), stdin);

        const char* jmeno = radek;

        fgets(radek, sizeof(radek), stdin);

        const char* prijmeni = radek;

        printf("%s", jmeno);
        printf("%s", prijmeni);

        return 0;
    }
    