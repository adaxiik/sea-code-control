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

            // const bool has_initializer;
            // const bool is_const;
            // const bool is_global;
            // const bool is_parameter;
            enum class Flags : uint8_t
            {
                None           = 1 << 0,
                HasInitializer = 1 << 1,
                IsConst        = 1 << 2,
                IsGlobal       = 1 << 3,
                IsParameter    = 1 << 4
            };

            
            const Flags flags;

            CreateValueVariableInstruction(const std::string& variable_name
                                         , Type variable_type
                                         , Flags flags)
            : variable_name(variable_name)
            , variable_type(variable_type)
            , flags(flags) {}
            
            InterpreterResult execute(InterpreterState &state) const;
        };

        CreateValueVariableInstruction::Flags operator|(CreateValueVariableInstruction::Flags lhs, CreateValueVariableInstruction::Flags rhs);
        bool operator&(CreateValueVariableInstruction::Flags lhs, CreateValueVariableInstruction::Flags rhs);
    }
} 
