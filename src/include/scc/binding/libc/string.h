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

unsigned long strlen(const char* str)
{
    char *s = str;
    while (*s) ++s;
    return s - str;
}

)";

}