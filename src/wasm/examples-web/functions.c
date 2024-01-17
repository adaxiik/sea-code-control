#include <math.h>

void _scc_puti(int x);
void _scc_putc(char x);

const float M_PI = 3.14159f;


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



void draw_sin_wave(int height, int width)
{
    float center = width / 2.0f;
    for (int y = 0; y < height; y += 1)
    {
        float sin_progress = (float)y / (float)height * 2.0f * M_PI;
        float sin_value = sinf(sin_progress);
        int x = (int)(center + sin_value * center);
        for (int i = 0; i < x; i += 1)
        {
            _scc_putc(' ');
        }
        _scc_putc('*');
        _scc_putc('\n');
    }
}


// =========== Main is here ===========
int main()
{
    print_hello();

    int factorial_of_5 = factorial(5);
    _scc_puti(factorial_of_5);

    float lerp_result = lerp(0.0f, 25.0f, 0.5f);

    print_pyramid(5);
    
    _scc_putc('\n');
    
    draw_sin_wave(15, 20);
    draw_sin_wave(15, 20);
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
