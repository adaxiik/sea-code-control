#pragma once

namespace scc::binding::libc
{

constexpr auto stdlib_h = R"(
#include <stddef.h>

void* _scc_malloc(size_t size);
void _scc_free(void* ptr);

void* malloc(size_t size)
{
    return _scc_malloc(size);
}

void free(void* ptr)
{
    _scc_free(ptr);
}


)";

}