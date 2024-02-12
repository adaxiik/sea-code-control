#pragma once

namespace scc::binding::libc
{

constexpr auto string_h = R"(
#include <stddef.h>

void* memset(void* s, int c, size_t n)
{
    char* p = (char*)s;
    for (size_t i = 0; i < n; i+=1){
        // p[i] = (char)c;
        *(p + i) = (char)c;
    }

    return s;
}

void* memcpy(void* dest, const void* src, size_t n)
{
    char* d = (char*)dest;
    char* s = (char*)src;
    for (size_t i = 0; i < n; i+=1){
        d[i] = s[i];
    }

    return dest;
}

size_t strlen(const char* str)
{
    char *s = str;
    while (*s){
        ++s;
    }
    return s - str;
}

)";

}