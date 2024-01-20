#include <stdio.h>

void _scc_puti(int x);

int g_a = 34;
int g_b = 35;

int g_default;  // global variables are initialized to 0


int foo = 420;

int main()
{
    _scc_puti(g_a);
    _scc_puti(g_b);
    _scc_puti(g_default);

    // =====
    for (int i = 0; i < 10; i+=1) {
        putchar('=');
    }
    putchar('\n');
    // =====


    _scc_puti(foo); // 420

    {
        int foo = 69; // shadowing
        _scc_puti(foo); // 69
    }

    _scc_puti(foo); // 420

    

    return 0;
}
