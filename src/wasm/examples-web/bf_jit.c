// optimizing bf jit compiler/interpreter :)

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <scc.h>

char *compiled_output;
unsigned long compiled_output_capacity = 0;
unsigned long compiled_output_size = 0;

void init_code()
{
    compiled_output = malloc(256);
    compiled_output_capacity = 256;
    compiled_output_size = 0;
}

void realloc_code_if_needed(unsigned long new_program_size)
{
    if (new_program_size <= compiled_output_capacity)
        return;

    unsigned long new_capacity = compiled_output_capacity * 2;
    while (new_program_size > new_capacity)
        new_capacity *= 2;

    char *new_output = malloc(new_capacity);
    memcpy(new_output, compiled_output, compiled_output_size);
    free(compiled_output);
    compiled_output = new_output;
    compiled_output_capacity = new_capacity;
}

void append_code(const char *code)
{
    unsigned long code_size = strlen(code) + 1;
    realloc_code_if_needed(compiled_output_size + code_size);
    memcpy(compiled_output + compiled_output_size, code, code_size);
    compiled_output_size += code_size - 1;
}

int eat_token(const char **input, char token)
{
    int count = 0;
    const char *ptr = *input;
    while (*ptr == token)
    {
        count+=1;
        ptr+=1;
    }
    *input = ptr;
    return count % 256; // 256 is the max value of a bf cell
}

void byte_to_str_binary(unsigned char n, char *output)
{
    // converting number to binary string is easier than decimal string :)
    output[0] = '0';
    output[1] = 'b';
    for (int i = 0; i < 8; i++)
    {
        output[9 - i] = (n & 1) + '0';
        n >>= 1;
    }
    output[10] = '\0';
}

void bf_compile(const char *input)
{
    char bin_buffer[11] = {0};

    append_code("#include <stdio.h>\n");
    append_code("int main() {\n");
    append_code("    char array[30000] = {0};\n");
    append_code("    char *ptr=array;\n");
    while (*input)
    {
        if (*input == '>')
        {
            int count = eat_token(&input, '>');
            byte_to_str_binary(count, bin_buffer);
            append_code("ptr += ");
            append_code(bin_buffer);
            append_code(";\n");
            continue;
        }
        else if (*input == '<')
        {
            int count = eat_token(&input, '<');
            byte_to_str_binary(count, bin_buffer);
            append_code("ptr -= ");
            append_code(bin_buffer);
            append_code(";\n");
            continue;
        }
        else if (*input == '+')
        {
            int count = eat_token(&input, '+');
            byte_to_str_binary(count, bin_buffer);
            append_code("*ptr += ");
            append_code(bin_buffer);
            append_code(";\n");
            continue;
        }
        else if (*input == '-')
        {
            int count = eat_token(&input, '-');
            byte_to_str_binary(count, bin_buffer);
            append_code("*ptr -= ");
            append_code(bin_buffer);
            append_code(";\n");
            continue;
        }
        else if (*input == '.')
        {
            append_code("putchar(*ptr);\n");
        }
        else if (*input == ',')
        {
            // input is not supported yet
        }
        else if (*input == '[')
        {
            append_code("while (*ptr) {\n");
        }
        else if (*input == ']')
        {
            append_code("}\n");
        }

        input++;
    }
    append_code("return 0;\n");
    append_code("}\n");

}

int main()
{
    const char *input = "++++++++++[>+++++++>++++++++++>+++>+<<<<-]>++.>+.+++++++..+++.>++.<<+++++++++++++++.>.+++.------.--------.>+.>.";

    init_code();
    bf_compile(input);

    // puts(compiled_output);

    if (scc_eval(compiled_output) == -1)
    {
        puts("Error");
    }

    free(compiled_output);
}
