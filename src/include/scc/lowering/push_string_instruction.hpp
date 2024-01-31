#pragma once
#include "interpreter_result.hpp"
#include "interpreter_state.hpp"

namespace scc
{
    namespace lowering
    {
        struct PushStringInstruction
        {
            const std::string value;
            PushStringInstruction(const std::string& value) : value(value) {}

            InterpreterResult execute(InterpreterState &state) const;
        };
    }
}
