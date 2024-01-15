#include <stdio.h>

void _scc_puti(int x);
void _scc_putc(char x);


int g;

int main()
{
    int* g_ptr = &g;
    *g_ptr = 420;
    _scc_puti(g);

    return 0;
}
