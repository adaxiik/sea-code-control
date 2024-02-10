#pragma once

namespace scc::binding::libc
{
constexpr auto stdbool_h = R"(

#  define true 1
#  define false 0

)";

}