#pragma once

namespace scc::binding::libc
{

constexpr auto stdio_h = R"(

long _scc_write(int fd, const void* data, unsigned long count);

int putchar(int c)
{
    // STDOUT_FILENO = 1
    return _scc_write(1, &c, 1);
}


)";

}