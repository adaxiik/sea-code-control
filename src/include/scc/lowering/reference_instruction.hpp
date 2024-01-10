#pragma once
#include "interpreter_state.hpp"

namespace scc
{
    namespace lowering
    {
        struct ReferenceInstruction
        {
            std::string identifier;
            ReferenceInstruction(std::string identifier) : identifier(identifier) {}
            InterpreterResult execute(InterpreterState &state) const;
        };
    }
} 
