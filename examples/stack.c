
#include <stdio.h>
#include <stdlib.h>

int main()
{
    int arr[5] = {1, 2, 3, 4, 5};
    
    int next;
    printf("%lu\n", &arr[0]);
    printf("%lu\n", &next);
    return 0;
}