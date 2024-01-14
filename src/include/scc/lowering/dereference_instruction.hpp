#pragma once
#include "interpreter_state.hpp"

namespace scc
{
    namespace lowering
    {
        struct DereferenceInstruction
        {
            Type type;
            DereferenceInstruction(Type type) : type(type) { }
            InterpreterResult execute(InterpreterState &state) const;
        };
    }
} 
