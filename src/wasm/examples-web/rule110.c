#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void next_generation(unsigned char *last_row, unsigned char* new_row, int width, unsigned char rule)
{
    for (int i = 0; i < width; ++i)
    {
        unsigned char left = last_row[(i - 1) % width];
        unsigned char me = last_row[i];
        unsigned char right = last_row[(i + 1) % width];

        int index = (left << 2) | (me << 1) | right;
        unsigned char true_or_false = (rule >> index) & 1;
        new_row[i] = true_or_false;
    }
}

void print_row(unsigned char *row, int width, char symbol)
{
    for (int i = 0; i < width; ++i)
    {
        char c = ' ';

        if (row[i])
            c = symbol;

        putchar(c);
    }
    putchar('\n');
}

void swap_rows(unsigned char **row, unsigned char **next_row)
{
    unsigned char *tmp = *row;
    *row = *next_row;
    *next_row = tmp;
}


#define WIDTH 40
#define HEIGHT 40
#define SYMBOL '#'


int main()
{
    // https://en.wikipedia.org/wiki/Rule_110

    const unsigned char rule = 110;

    unsigned char row_data[WIDTH] = {0};
    unsigned char next_row_data[WIDTH] = {0};

    // we'll use pointers to swap rows later
    unsigned char *row = row_data;
    unsigned char *next_row = next_row_data;

    // we'll set our starting state
    row[WIDTH - 1] = 1;

    // first generation
    print_row(row, WIDTH, SYMBOL);

    for (int i = 0; i < HEIGHT; ++i)
    {
        next_generation(row, next_row, WIDTH, rule);
        print_row(next_row, WIDTH, SYMBOL);
        swap_rows(&row, &next_row);
    }

    return 0;
}
