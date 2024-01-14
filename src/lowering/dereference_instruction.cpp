#include "lowering/dereference_instruction.hpp"
#include "variable.hpp"
namespace scc
{
    namespace lowering
    {
        InterpreterResult DereferenceInstruction::execute(InterpreterState &state) const
        {
            if (state.result_stack.size() < 1)
                return InterpreterResult::error(InterpreterError::RuntimeError);

            auto address = state.result_stack.top();
            state.result_stack.pop();

            if (not address.has_value())
                return InterpreterResult::error(InterpreterError::RuntimeError);

            // this is already checked in binder
            if (not address.get_value().type.is_pointer())
                return InterpreterResult::error(InterpreterError::ReachedUnreachableError);
            
            if (not std::holds_alternative<Type::PrimitiveValue>(address.get_value().value.value))
                return InterpreterResult::error(InterpreterError::DereferenceError);


            auto primitive_val = std::get<Type::PrimitiveValue>(address.get_value().value.value);
            if (not std::holds_alternative<Type::Primitive::U64>(primitive_val))
                return InterpreterResult::error(InterpreterError::DereferenceError);
            
            auto address_value = std::get<Type::Primitive::U64>(primitive_val);

            auto value = state.memory.read_value(address_value, type);

            if (not value)
                return InterpreterResult::error(InterpreterError::MemoryError);
            
            return InterpreterResult::ok(InterpreterResultValue(value.value(), type));
        }
    }
}