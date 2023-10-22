#pragma once
#include <functional>
#include "interpreter_state.hpp"
namespace scc
{
    namespace lowering
    {
        struct CallInbuildInstruction
        {
            std::string function_name;
            std::function<InterpreterResult(InterpreterState&)> fn;
            bool have_return_value;

            CallInbuildInstruction(const std::string& function_name, std::function<InterpreterResult(InterpreterState&)> fn, bool have_return_value)
            : function_name(function_name)
            , fn(fn)
            , have_return_value(have_return_value)
            {}

            InterpreterResult execute(InterpreterState &state) const;
        };
    }
} 
