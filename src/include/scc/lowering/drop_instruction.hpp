#pragma once
#include "interpreter_result.hpp"
#include "interpreter_state.hpp"
namespace scc
{
    namespace lowering
    {
        struct DropInstruction
        {
            const size_t count;
            DropInstruction(size_t count = 1) : count(count) {}
            InterpreterResult execute(InterpreterState &state) const;
        };
    }
} 
