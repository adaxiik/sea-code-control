#include <stdio.h>
#include <scc.h>

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
        scc_puti(i);
    }


    int max = 5;
    while (max -= 1)
    {
        scc_puti(max);
    }

    return 0;
}
