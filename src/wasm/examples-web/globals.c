#include <stdio.h>

void _scc_puti(int x);
void _scc_putc(char x);

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
        _scc_putc('=');
    }
    _scc_putc('\n');
    // =====


    _scc_puti(foo); // 420

    {
        int foo = 69; // shadowing
        _scc_puti(foo); // 69
    }

    _scc_puti(foo); // 420

    

    return 0;
}
