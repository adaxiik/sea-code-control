#pragma once
#include "type.hpp"
#include "relptr_t.hpp"

namespace scc
{
    namespace vm
    {
        struct Variable
        {
            type::Type type;
            relptr_t pointer;
        };
    }
}