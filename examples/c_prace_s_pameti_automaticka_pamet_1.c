// https://mrlvsb.github.io/upr-skripta/c/prace_s_pameti/automaticka_pamet.html, 1. runnable code block

#include <stdio.h>

int fun1(int par) {
    int res = par * 2;
    if (res < 50) {
        return fun1(res);
    }
    else { return res; }
}
int fun2(int a, int b) {
    int x = a + b * 2;
    int y = fun1(x);
    return x + y;
}
int main() {
    printf("%d\n", fun2(5, 6));
    return 0;
}