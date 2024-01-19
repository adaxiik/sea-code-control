#pragma once

namespace scc::binding::libc
{

constexpr auto string_h = R"(

void* memset(void* s, int c, long n)
{
    char* p = (char*)s;
    for (long i = 0; i < n; i+=1){
        // p[i] = (char)c;
        *(p + i) = (char)c;
    }

    return s;
}


long _scc_write(int fd, const void* data, unsigned long count);

int putchar(int c)
{
    // STDOUT_FILENO = 1
    return _scc_write(1, &c, 1);
}

)";

}