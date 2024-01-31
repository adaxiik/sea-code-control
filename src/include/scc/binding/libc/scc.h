#pragma once

namespace scc::binding::libc
{

constexpr auto scc_h = R"(
void _scc_puti(int x);

void scc_puti(int x)
{
    _scc_puti(x);
}

)";

}