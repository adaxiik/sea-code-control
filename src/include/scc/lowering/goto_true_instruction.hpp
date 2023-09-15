#pragma once
#include "interpreter_result.hpp"
#include "interpreter_state.hpp"
namespace scc
{
    namespace lowering
    {
        struct GotoTrueInstruction
        {
            using Label = size_t;
            Label label;
            GotoTrueInstruction(Label label);
            InterpreterResult execute(InterpreterState &state) const;
        };
    }
} 
