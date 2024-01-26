#include "lowering/create_array_variable_instruction.hpp"

namespace scc
{
    namespace lowering
    {
        InterpreterResult CreateArrayVariableInstruction::execute(InterpreterState &state) const
        {
            InterpreterResult creation_result = InterpreterError::None;
            if (not (flags & Flags::IsGlobal))
            {
                creation_result = state.call_stack.scope_stack().create_variable(
                    variable_name
                    , variable_type
                    , variable_type.size_bytes()
                    , flags & Flags::IsConst
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

            auto variable = flags & Flags::IsGlobal
             ? state.global_scope.get_variable(variable_name) 
             : state.call_stack.scope_stack().get_from_scopestack(variable_name);

            if (not variable)
                return InterpreterError::VariableDoesntExistError;
            


            // int a[5];
            // a == &a
            if (not variable->set_value(state.memory, Type::Value(static_cast<Type::Primitive::PTR>(variable->address()))))
                return creation_result;

            if (not (flags & Flags::HasInitializer))
                return creation_result;
            
            if (state.result_stack.size() < initializer_size_elements)
                return InterpreterError::RuntimeError;


            Type underlying_type = variable_type;
            underlying_type.modifiers.pop_back();
            size_t underlying_type_size = underlying_type.size_bytes();
            for (uint32_t i = 0; i < initializer_size_elements; ++i)
            {
                auto value = state.result_stack.top().get_value();
                state.result_stack.pop();

                if (not state.memory.write_value(variable->address() + i * underlying_type_size, value.value, underlying_type))
                    return InterpreterError::MemoryError;
            }

            size_t initialized_bytes = initializer_size_elements * underlying_type_size;
            size_t uninitialized_bytes = variable_type.size_bytes() - initialized_bytes;

            if (state.memory.memset(variable->address() + initialized_bytes, 0, uninitialized_bytes))
                return creation_result;
            
            
            return InterpreterError::RuntimeError;
        }

        CreateArrayVariableInstruction::Flags operator|(CreateArrayVariableInstruction::Flags lhs, CreateArrayVariableInstruction::Flags rhs)
        {
            return static_cast<CreateArrayVariableInstruction::Flags>(static_cast<uint8_t>(lhs) | static_cast<uint8_t>(rhs));
        }

        bool operator&(CreateArrayVariableInstruction::Flags lhs, CreateArrayVariableInstruction::Flags rhs)
        {
            return static_cast<uint8_t>(lhs) & static_cast<uint8_t>(rhs);
        }
    }
}