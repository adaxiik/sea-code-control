#pragma once

namespace scc::binding::libc
{

constexpr auto stdio_h = R"(

#include <unistd.h>
#include <string.h>

int putchar(int c)
{
    return write(STDOUT_FILENO, &c, 1);
}

int puts(const char* s)
{
    int w = write(STDOUT_FILENO, s, strlen(s));
    putchar('\n');
    return w;
}

)";

}