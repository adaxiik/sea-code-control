#pragma once

namespace scc::binding::libc
{

constexpr auto scc_h = R"(
#include <string.h>

void _scc_puti(int x);
int _scc_eval(const char *s, unsigned long len);

void scc_puti(int x)
{
    _scc_puti(x);
}

int scc_eval(const char* s)
{
    return _scc_eval(s, strlen(s));
}

int scc_eval_len(const char* s, unsigned long len)
{
    return _scc_eval(s, len);
}

)";

}