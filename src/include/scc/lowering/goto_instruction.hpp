#pragma once
#include "interpreter_result.hpp"
#include "interpreter_state.hpp"
namespace scc
{
    namespace lowering
    {
        struct GotoInstruction
        {
            using Label = size_t;
            Label label;
            GotoInstruction(Label label);
            InterpreterResult execute(InterpreterState &state) const;
        };
    }
} 
