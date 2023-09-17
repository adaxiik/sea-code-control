#pragma once
#include "interpreter_result.hpp"
#include "interpreter_state.hpp"

namespace scc
{
    namespace lowering
    {
        struct PushLiteralInstruction
        {
            const Type type;
            Type::Value value;
            PushLiteralInstruction(const Type& type, Type::Value value)
            : type(type)
            , value(value) {}
            InterpreterResult execute(InterpreterState &state) const;
        };
    }
} 
