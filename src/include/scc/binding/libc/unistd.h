#pragma once

namespace scc::binding::libc
{

constexpr auto unistd_h = R"(
#include <stddef.h>

#define EOF -1
#define STDIN_FILENO 0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2

ssize_t _scc_write(int fd, const void* data, size_t count);


ssize_t write(int fd, const void* data, size_t count)
{
    return _scc_write(fd, data, count);
}

)";

}