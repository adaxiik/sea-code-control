#pragma once
#include <string>
#include "interpreter_result.hpp"
#include "interpreter_state.hpp"

namespace scc
{
    namespace lowering
    {
        struct AssignmentInstruction
        {
            std::string variable_name;

            AssignmentInstruction(const std::string &name);
            InterpreterResult execute(InterpreterState &state) const;
        };
    }
} 
