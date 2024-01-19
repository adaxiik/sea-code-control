#pragma once

namespace scc::binding::libc
{

constexpr auto unistd_h = R"(

#define EOF -1
#define STDIN_FILENO 0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2

long _scc_write(int fd, const void* data, unsigned long count);


long write(int fd, const void* data, unsigned long count)
{
    return _scc_write(fd, data, count);
}

)";

}