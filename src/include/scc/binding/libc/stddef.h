#pragma once

namespace scc::binding::libc
{
constexpr auto stddef_h = R"(

#define NULL ((void*)0)
typedef unsigned long size_t;
typedef long ssize_t;

)";

}