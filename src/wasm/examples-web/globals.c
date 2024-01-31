#include <stdio.h>
#include <scc.h>

int g_a = 34;
int g_b = 35;

int g_default;  // global variables are initialized to 0


int foo = 420;

int main()
{
    scc_puti(g_a);
    scc_puti(g_b);
    scc_puti(g_default);

    // =====
    for (int i = 0; i < 10; ++i) {
        putchar('=');
    }
    putchar('\n');
    // =====


    scc_puti(foo); // 420

    {
        int foo = 69; // shadowing
        scc_puti(foo); // 69
    }

    scc_puti(foo); // 420



    return 0;
}
