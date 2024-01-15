#pragma once
#include <string>
#include "interpreter_result.hpp"
#include "interpreter_state.hpp"
#include "type.hpp"

namespace scc
{
    namespace lowering
    {
        struct PointerAssignmentInstruction
        {
            Type target_type;
            PointerAssignmentInstruction(Type target_type)
                : target_type(target_type) {}
            InterpreterResult execute(InterpreterState &state) const;
        };
    }
} 
