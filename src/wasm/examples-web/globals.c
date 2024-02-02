#include <stdio.h>

int g_a = 34;
int g_b = 35;

int g_default;  // global variables are initialized to 0


int foo = 420;

int main()
{
    printf("g_a = %d\ng_b = %d\ng_default = %d\n", g_a, g_b, g_default);

    puts("=========================");

    printf("%d\n", foo); // 420

    {
        int foo = 69; // shadowing
        printf("%d\n", foo); // 69
    }

    printf("%d\n", foo); // 420


    return 0;
}
