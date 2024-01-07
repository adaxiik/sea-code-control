#include <stdio.h>

void _scc_puti(int x);
void _scc_putc(char x);

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
    for (int i = 0; i < height; i += 1)
    {
        for (int j = 0; j < height - i; j += 1)
        {
            _scc_putc(' ');
        }
        for (int j = 0; j < i * 2 + 1; j += 1)
        {
            _scc_putc('*');
        }
        _scc_putc('\n');
    }
}

int recursive_fibonacci(int n)
{
    if (n <= 1)
        return n;
    return recursive_fibonacci(n - 1) + recursive_fibonacci(n - 2);
}

// =========== Main is here ===========
int main()
{
    print_hello();

    int factorial_of_5 = factorial(5);
    _scc_puti(factorial_of_5);

    float lerp_result = lerp(0.0f, 25.0f, 0.5f);

    print_pyramid(5);


    return 0;
}
// ====================================

void print_hello()
{
    _scc_putc('H');
    _scc_putc('e');
    _scc_putc('l');
    _scc_putc('l');
    _scc_putc('o');
    _scc_putc(' ');
    _scc_putc('W');
    _scc_putc('o');
    _scc_putc('r');
    _scc_putc('l');
    _scc_putc('d');
    _scc_putc('!');
    _scc_putc('\n');
}