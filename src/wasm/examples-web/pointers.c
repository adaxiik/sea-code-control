#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void _scc_puti(int x);
void _scc_putc(char x);



int g;

int main()
{
    int* g_ptr = &g;
    *g_ptr = 420;
    _scc_puti(g);

    int val_a = 64;
    int val_b = 420;
    int* ptr_a = &val_a;
    int* ptr_b = &val_b;
    int** ptr = &ptr_a;

    _scc_puti(**ptr);
    ptr = &ptr_b;
    _scc_puti(**ptr);

    // heap
    char* data = malloc(4);
    *(data + 0) = 'l';
    *(data + 1) = 'o';
    *(data + 2) = 'l';
    *(data + 3) = '\n';
    write(STDOUT_FILENO, data, 4);

    free(data);

    return 0;
}
