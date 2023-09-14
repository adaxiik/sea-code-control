#pragma once
#include "interpreter_result.hpp"
#include "interpreter_state.hpp"


namespace scc
{
    namespace lowering
    {
        struct PopScopeInstruction
        {
            PopScopeInstruction() = default;
            InterpreterResult execute(InterpreterState &state) const;
        };
    }
} 
