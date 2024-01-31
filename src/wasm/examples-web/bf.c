#include <stdio.h>

void interpret(char *input)
{
    char tape[30000] = {0};
    char *ptr = tape;

    for (int i = 0; input[i]; ++i)
    {
        char command = input[i];
        if (command == '>')
        {
            ++ptr;
        }
        else if (command == '<')
        {
            --ptr;
        }
        else if (command == '+')
        {
            ++*ptr;
        }
        else if (command == '-')
        {
            --*ptr;
        }
        else if (command == '.')
        {
            putchar(*ptr);
        }
        else if (command == ',')
        {
            // input is not implemented yet
        }
        else if (command == '[')
        {
            if (*ptr == 0)
            {
                int loop_count = 1;
                while (loop_count > 0)
                {
                    ++i;
                    if (input[i] == '[')
                        ++loop_count;
                    else if (input[i] == ']')
                        --loop_count;
                }
            }
        }
        else if (command == ']')
        {
            if (*ptr)
            {
                int loop_count = 1;
                while (loop_count > 0)
                {
                    --i;
                    if (input[i] == '[')
                        --loop_count;
                    else if (input[i] == ']')
                        ++loop_count;
                }
            }
        }
    }
}

int main()
{
    const char* input = "++++++++++[>+++++++>++++++++++>+++>+<<<<-]>++.>+.+++++++..+++.>++.<<+++++++++++++++.>.+++.------.--------.>+.>.";
    interpret(input);
    return 0;
}
