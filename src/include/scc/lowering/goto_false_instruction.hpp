#pragma once
#include "interpreter_result.hpp"
#include "interpreter_state.hpp"
namespace scc
{
    namespace lowering
    {
        struct GotoFalseInstruction
        {
            using Label = size_t;
            Label label;
            GotoFalseInstruction(Label label);
            InterpreterResult execute(InterpreterState &state) const;
        };
    }
} 
