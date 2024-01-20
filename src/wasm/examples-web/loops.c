#include <stdio.h>

void _scc_puti(int x);  // print integer with newline

int main()
{
    for (int i = 0; i < 10; ++i)
    {
        putchar('=');
    }
    putchar('\n');

    for (;;)
    {
        putchar('x');
        putchar('\n');
        break;
    }

    for (int i = 0; i < 10; ++i)
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
