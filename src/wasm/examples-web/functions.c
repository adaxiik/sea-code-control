#include <math.h>
#include <stdio.h>
#include <scc.h>


void print_hello(); // defined later

int factorial(int n)
{
    if (n <= 0)
        return 1;

    // I am assigning the variable only so that it can be seen in the visualization :)
    int result = n * factorial(n - 1);
    return result;
}

float lerp(float a, float b, float t)
{
    return a + (b - a) * t;
}

void print_pyramid(int height)
{
    for (int i = 0; i < height; ++i)
    {
        for (int j = 0; j < height - i; ++j)
        {
            putchar(' ');
        }
        for (int j = 0; j < i * 2 + 1; ++j)
        {
            putchar('*');
        }
        putchar('\n');
    }
}



void draw_sin_wave(int height, int width)
{
    float center = width / 2.0f;
    for (int y = 0; y < height; ++y)
    {
        float sin_progress = (float)y / (float)height * 2.0f * M_PI;
        float sin_value = sinf(sin_progress);
        int x = (int)(center + sin_value * center);
        for (int i = 0; i < x; ++i)
        {
            putchar(' ');
        }
        putchar('*');
        putchar('\n');
    }
}


// =========== Main is here ===========
int main()
{
    print_hello();

    int factorial_of_5 = factorial(5);
    scc_puti(factorial_of_5);

    float lerp_result = lerp(0.0f, 25.0f, 0.5f);

    print_pyramid(5);

    putchar('\n');

    draw_sin_wave(15, 20);
    draw_sin_wave(15, 20);
    return 0;
}
// ====================================

void print_hello()
{
    putchar('H');
    putchar('e');
    putchar('l');
    putchar('l');
    putchar('o');
    putchar(' ');
    putchar('W');
    putchar('o');
    putchar('r');
    putchar('l');
    putchar('d');
    putchar('!');
    putchar('\n');
}
