#pragma once

namespace scc::binding::libc
{

constexpr auto stdlib_h = R"(

void* _scc_malloc(unsigned long size);
void _scc_free(void* ptr);

void* malloc(unsigned long size)
{
    return _scc_malloc(size);
}

void free(void* ptr)
{
    _scc_free(ptr);
}


)";

}