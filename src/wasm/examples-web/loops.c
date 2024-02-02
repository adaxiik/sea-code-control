#include <stdio.h>

int main()
{
    for (int i = 0; i < 10; ++i)
    {
        putchar('=');
    }
    putchar('\n');

    for (;;)
    {
        printf("x\n");
        break;
    }

    for (int i = 0; i < 10; ++i)
    {
        if (i % 2 == 0)
            continue;

        printf("i = %d\n", i);
    }


    int max = 5;
    while (max -= 1)
    {
        printf("max = %d\n", max);
    }

    return 0;
}
