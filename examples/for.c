#include <stdio.h>

void _scc_puti(int x);
void _scc_putc(char x);


void print_num(int x)
{
    _scc_puti(x);
}


int main()
{
    for (int i = 0; i < 5; i+=1) {
        print_num(i);
    }

    for(;;)
    {
        print_num(69);
        break;
    }

    return 0;
}
