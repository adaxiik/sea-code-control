#pragma once
#include "interpreter_result.hpp"
#include "interpreter_state.hpp"
#include "binding/bound_cast_expression.hpp"

namespace scc
{
    namespace lowering
    {
        struct CastInstruction
        {
            const Type type;
            CastInstruction(Type type) : type(type) {}
            InterpreterResult execute(InterpreterState &state) const;
        };
    }
} 
