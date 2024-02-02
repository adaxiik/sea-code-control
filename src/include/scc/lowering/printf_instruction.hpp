#pragma once
#include "interpreter_result.hpp"
#include "interpreter_state.hpp"

namespace scc
{
    namespace lowering
    {
        struct PrintfInstruction
        {
            size_t argument_count_without_fmt;
            PrintfInstruction(size_t argument_count_without_fmt) : argument_count_without_fmt(argument_count_without_fmt) {}
            InterpreterResult execute(InterpreterState &state) const;
        };
    }
}
