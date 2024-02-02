#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int g;

int main()
{
    int* g_ptr = &g;
    *g_ptr = 420;
    printf("g = %d\n", g);

    int val_a = 64;
    int val_b = 420;
    int* ptr_a = &val_a;
    int* ptr_b = &val_b;
    int** ptr = &ptr_a;

    printf("**ptr = %d\n", **ptr);
    ptr = &ptr_b;
    printf("**ptr = %d\n", **ptr);

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
        printf("i = %d\n", stack_data[i]);
    }

    return 0;
}
