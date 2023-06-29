
#include <stdio.h>
#include <stdlib.h>

void test(int size)
{
    char arr[size];
    char next;
    printf("%p\n", (void*)&arr[0]);
    printf("%p\n", (void*)&next);
}


int main()
{
    int arr[5] = {1, 2, 3, 4, 5};
    
    int next;
    printf("%p\n", (void*)&arr[0]);
    printf("%p\n", (void*)&next);
    return 0;
}
