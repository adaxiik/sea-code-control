#pragma once

namespace scc::binding::libc
{

constexpr auto unistd_h = R"(

const int EOF = -1;
const int stdin = 0;
const int stdout = 1;
const int stderr = 2;

long _scc_write(int fd, const void* data, unsigned long count);


long write(int fd, const void* data, unsigned long count)
{
    return _scc_write(fd, data, count);
}

)";

}