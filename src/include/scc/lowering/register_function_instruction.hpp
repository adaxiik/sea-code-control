#pragma once
#include "interpreter_state.hpp"

namespace scc
{
    namespace lowering
    {
        struct RegisterFunctionInstruction
        {
            std::string function_name;
            RegisterFunctionInstruction(std::string function_name) : function_name(function_name) {}
            InterpreterResult execute(InterpreterState &state) const;
        };
    }
} 
