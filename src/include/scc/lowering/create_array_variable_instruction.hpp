#pragma once
#include <string>
#include "interpreter_result.hpp"
#include "interpreter_state.hpp"
#include "type.hpp"

namespace scc
{
    namespace lowering
    {
        struct CreateArrayVariableInstruction
        {
            const std::string variable_name;
            const Type variable_type;
            const uint32_t initializer_size_elements;

            enum class Flags : uint8_t
            {
                None           = 1 << 0,
                HasInitializer = 1 << 1,
                IsConst        = 1 << 2,
                IsGlobal       = 1 << 3,
            };

            const Flags flags;

            CreateArrayVariableInstruction(const std::string& variable_name
                                         , Type variable_type
                                         , Flags flags
                                         , size_t initializer_size_elements)
            : variable_name(variable_name)
            , variable_type(variable_type)
            , initializer_size_elements(static_cast<uint32_t>(initializer_size_elements))
            , flags(flags)
            {}
            
            InterpreterResult execute(InterpreterState &state) const;
        };

        CreateArrayVariableInstruction::Flags operator|(CreateArrayVariableInstruction::Flags lhs, CreateArrayVariableInstruction::Flags rhs);
        bool operator&(CreateArrayVariableInstruction::Flags lhs, CreateArrayVariableInstruction::Flags rhs);
    }
} 
