#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define SIZE_OF_CHAR 1

void _scc_puti(int i);

void next_generation(unsigned char *last_row, unsigned char* new_row, int width, unsigned char rule)
{
    for (int i = 0; i < width; i+=1)
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
    // TODO: implement buffering with static array
    for (int i = 0; i < width; i+=1)
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

int main()
{
    // https://en.wikipedia.org/wiki/Rule_110
    
    const int width = 40;
    const int height = 40;
    const char symbol = '#';

    unsigned char rule = 110;
    unsigned char *row = (unsigned char *)malloc(width * SIZE_OF_CHAR); 
    unsigned char *next_row = (unsigned char *)malloc(width * SIZE_OF_CHAR);

    memset(row, 0, width * SIZE_OF_CHAR);

    // we will set our starting state
    row[width - 1] = 1;

    // first generation
    print_row(row, width, symbol);

    for (int i = 0; i < height; i+=1)
    {
        next_generation(row, next_row, width, rule);
        print_row(next_row, width, symbol);
        swap_rows(&row, &next_row);
    }

    free(row);
    free(next_row);

    return 0;
}
