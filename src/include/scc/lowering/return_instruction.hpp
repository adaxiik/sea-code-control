#pragma once
#include "interpreter_result.hpp"
#include "interpreter_state.hpp"

namespace scc
{
    namespace lowering
    {
        struct ReturnInstruction
        {
            bool has_return_expression;
            ReturnInstruction(bool has_return_expression = false)
                : has_return_expression(has_return_expression)
            {
            }
            InterpreterResult execute(InterpreterState &state) const;
        };
    }
} 
