#include "lowering/create_value_variable_instruction.hpp"

namespace scc
{
    namespace lowering
    {
        InterpreterResult CreateValueVariableInstruction::execute(InterpreterState &state) const
        {
            InterpreterResult creation_result = InterpreterError::None;
            if (not (flags & Flags::IsGlobal))
            {
                creation_result = state.call_stack.scope_stack().create_variable(
                    variable_name
                    , variable_type
                    , variable_type.size_bytes()
                    , flags & Flags::IsConst
                    , flags & Flags::IsParameter
                );
            }
            else
            {
                creation_result = state.global_scope.create_variable(
                    variable_name
                    , variable_type
                    , variable_type.size_bytes()
                    , flags & Flags::IsConst
                );
            }

            if (creation_result.is_error())
                return creation_result;

            if (not (flags & Flags::HasInitializer))
                return creation_result;

            auto variable = flags & Flags::IsGlobal
             ? state.global_scope.get_variable(variable_name) 
             : state.call_stack.scope_stack().get_from_scopestack(variable_name);

            if (not variable)
                return InterpreterError::VariableDoesntExistError;
            
            if (state.result_stack.size() < 1)
                return InterpreterError::RuntimeError;

            auto value = state.result_stack.top();
            state.result_stack.pop();

            if (variable->set_value(state.memory, value.get_value().value))
                return creation_result;
                
            return InterpreterError::RuntimeError;
        }

        CreateValueVariableInstruction::Flags operator|(CreateValueVariableInstruction::Flags lhs, CreateValueVariableInstruction::Flags rhs)
        {
            return static_cast<CreateValueVariableInstruction::Flags>(static_cast<uint8_t>(lhs) | static_cast<uint8_t>(rhs));
        }

        bool operator&(CreateValueVariableInstruction::Flags lhs, CreateValueVariableInstruction::Flags rhs)
        {
            return static_cast<uint8_t>(lhs) & static_cast<uint8_t>(rhs);
        }
    }
}