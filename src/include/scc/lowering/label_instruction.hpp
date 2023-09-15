#pragma once
#include "interpreter_result.hpp"
#include "interpreter_state.hpp"
namespace scc
{
    namespace lowering
    {
        struct LabelInstruction
        {
            using Label = size_t;
            Label label;
            LabelInstruction(Label label);
            InterpreterResult execute(InterpreterState &state) const;
        };
    }
} 
