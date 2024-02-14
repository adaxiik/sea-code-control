// optimizing bf jit compiler/interpreter :)

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <scc.h>

typedef struct {
    char *ptr;
    size_t capacity;
    size_t size;
} CodeBuffer;

void init_code(CodeBuffer *buffer)
{
    buffer->ptr = malloc(256);
    buffer->capacity = 256;
    buffer->size = 0;
}

void free_code(CodeBuffer *buffer)
{
    free(buffer->ptr);
    buffer->ptr = NULL;
}

void realloc_code_if_needed(CodeBuffer *buffer, size_t new_program_size)
{
    if (new_program_size <= buffer->capacity)
        return;

    size_t new_capacity = buffer->capacity * 2;
    while (new_program_size > new_capacity)
        new_capacity *= 2;

    char *new_output = malloc(new_capacity);
    memcpy(new_output, buffer->ptr, buffer->size);
    free(buffer->ptr);
    buffer->ptr = new_output;
    buffer->capacity = new_capacity;
}

void append_code(CodeBuffer *buffer, const char *code)
{
    size_t code_size = strlen(code) + 1;
    realloc_code_if_needed(buffer, buffer->size + code_size);
    memcpy(buffer->ptr + buffer->size, code, code_size);
    buffer->size += code_size - 1;
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

void bf_compile(CodeBuffer *buffer, const char *input)
{
    char bin_buffer[11] = {0};

    append_code(buffer, "#include <stdio.h>\n");
    append_code(buffer, "int main() {\n");
    append_code(buffer, "    char array[30000] = {0};\n");
    append_code(buffer, "    char *ptr=array;\n");
    while (*input)
    {
        if (*input == '>')
        {
            int count = eat_token(&input, '>');
            byte_to_str_binary(count, bin_buffer);
            append_code(buffer, "ptr += ");
            append_code(buffer, bin_buffer);
            append_code(buffer, ";\n");
            continue;
        }
        else if (*input == '<')
        {
            int count = eat_token(&input, '<');
            byte_to_str_binary(count, bin_buffer);
            append_code(buffer, "ptr -= ");
            append_code(buffer, bin_buffer);
            append_code(buffer, ";\n");
            continue;
        }
        else if (*input == '+')
        {
            int count = eat_token(&input, '+');
            byte_to_str_binary(count, bin_buffer);
            append_code(buffer, "*ptr += ");
            append_code(buffer, bin_buffer);
            append_code(buffer, ";\n");
            continue;
        }
        else if (*input == '-')
        {
            int count = eat_token(&input, '-');
            byte_to_str_binary(count, bin_buffer);
            append_code(buffer, "*ptr -= ");
            append_code(buffer, bin_buffer);
            append_code(buffer, ";\n");
            continue;
        }
        else if (*input == '.')
        {
            append_code(buffer, "putchar(*ptr);\n");
        }
        else if (*input == ',')
        {
            // input is not supported yet
        }
        else if (*input == '[')
        {
            append_code(buffer, "while (*ptr) {\n");
        }
        else if (*input == ']')
        {
            append_code(buffer, "}\n");
        }

        input++;
    }
    append_code(buffer, "return 0;\n");
    append_code(buffer, "}\n");

}

int main()
{
    const char *input = "++++++++++[>+++++++>++++++++++>+++>+<<<<-]>++.>+.+++++++..+++.>++.<<+++++++++++++++.>.+++.------.--------.>+.>.";

    CodeBuffer buffer;
    init_code(&buffer);
    bf_compile(&buffer, input);

    // puts(buffer.ptr);

    if (scc_eval(buffer.ptr) == -1)
    {
        puts("Error");
    }

    free_code(&buffer);
}
