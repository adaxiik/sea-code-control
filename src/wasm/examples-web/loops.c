#include <stdio.h>

void _scc_puti(int x);  // print integer with newline
void _scc_putc(char x); // print character

int main()
{
    for (int i = 0; i < 10; i += 1)
    {
        _scc_putc('=');
    }
    _scc_putc('\n');

    for (;;)
    {
        _scc_putc('x');
        _scc_putc('\n');
        break;
    }

    for (int i = 0; i < 10; i += 1)
    {
        if (i % 2 == 0)
            continue;
        _scc_puti(i);
    }


    int max = 5;
    while (max -= 1)
    {
        _scc_puti(max);
    }

    return 0;
}
