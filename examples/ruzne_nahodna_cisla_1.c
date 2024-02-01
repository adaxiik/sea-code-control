// https://mrlvsb.github.io/upr-skripta/ruzne/nahodna_cisla.html, 1. runnable code block

int S = 5;
int vygeneruj_cislo() {
    int N = S;
    N = (5 * N + 3) % 6323;
    N = (4 * N + 2) % 8127;
    S = N;
    return N;
}
int main() {
    int r1 = vygeneruj_cislo(); // 114
    int r2 = vygeneruj_cislo(); // 2294
    int r3 = vygeneruj_cislo(); // 4348
    int r4 = vygeneruj_cislo(); // 2971
    int r5 = vygeneruj_cislo(); // 723
    return 0;
}