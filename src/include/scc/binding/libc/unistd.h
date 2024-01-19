#pragma once

namespace scc::binding::libc
{

constexpr auto unistd_h = R"(

#define EOF -1
#define stdin 0
#define stdout 1
#define stderr 2

long _scc_write(int fd, const void* data, unsigned long count);


long write(int fd, const void* data, unsigned long count)
{
    return _scc_write(fd, data, count);
}

)";

}