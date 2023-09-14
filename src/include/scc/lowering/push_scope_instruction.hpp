#pragma once
#include "interpreter_state.hpp"

namespace scc
{
    namespace lowering
    {
        struct PushScopeInstruction
        {
            PushScopeInstruction() = default;
            InterpreterResult execute(InterpreterState &state) const;
        };
    }
} 
