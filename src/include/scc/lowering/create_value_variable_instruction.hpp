#pragma once
#include <string>
#include "interpreter_result.hpp"
#include "interpreter_state.hpp"
#include "type.hpp"

namespace scc
{
    namespace lowering
    {
        struct CreateValueVariableInstruction
        {
            const std::string variable_name;
            const Type variable_type;
            const bool has_initializer;
            const bool is_const;
            CreateValueVariableInstruction(const std::string& variable_name, Type variable_type, bool has_initializer, bool is_const)
            : variable_name(variable_name)
            , variable_type(variable_type)
            , has_initializer(has_initializer)
            , is_const(is_const){}
            
            InterpreterResult execute(InterpreterState &state) const;
        };
    }
} 
