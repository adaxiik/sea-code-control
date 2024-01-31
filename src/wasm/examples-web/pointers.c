#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <scc.h>

int g;

int main()
{
    int* g_ptr = &g;
    *g_ptr = 420;
    scc_puti(g);

    int val_a = 64;
    int val_b = 420;
    int* ptr_a = &val_a;
    int* ptr_b = &val_b;
    int** ptr = &ptr_a;

    scc_puti(**ptr);
    ptr = &ptr_b;
    scc_puti(**ptr);

    // heap
    char* data = malloc(4);
    *(data + 0) = 'l';
    *(data + 1) = 'o';
    *(data + 2) = 'l';
    *(data + 3) = '\n';
    write(STDOUT_FILENO, data, 4);

    free(data); // don't forget to free :)

    // stack
    int stack_data[] = {1, 2, 3, 4};
    for (int i = 0; i < 4; ++i){
        scc_puti(stack_data[i]);
    }

    return 0;
}
