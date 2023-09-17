#pragma once
#include "interpreter_result.hpp"
#include "interpreter_state.hpp"
#include "type.hpp"
#include <string>

namespace scc
{
    namespace lowering
    {
        struct IdentifierInstruction
        {
            std::string identifier;
            Type type;
            IdentifierInstruction(const std::string& identifier, Type type) : identifier(identifier), type(type) {}
            InterpreterResult execute(InterpreterState &state) const;
        };
    }
} 
