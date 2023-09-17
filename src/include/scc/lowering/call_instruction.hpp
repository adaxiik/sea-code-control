#pragma once
#include "interpreter_state.hpp"

namespace scc
{
    namespace lowering
    {
        struct CallInstruction
        {
            std::string function_name;
            bool have_return_value;

            CallInstruction(std::string function_name, bool have_return_value) : function_name(function_name), have_return_value(have_return_value)
            {
            }
            InterpreterResult execute(InterpreterState &state) const;
        };
    }
} 
