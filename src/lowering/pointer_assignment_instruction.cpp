#include "lowering/pointer_assignment_instruction.hpp"

namespace scc
{
    namespace lowering
    {
        InterpreterResult PointerAssignmentInstruction::execute(InterpreterState &state) const
        {
            if (state.result_stack.size() < 2)
                return InterpreterError::RuntimeError;

            auto result_to_assign = state.result_stack.top();
            state.result_stack.pop();

            auto pointer = state.result_stack.top();
            state.result_stack.pop();

            if (not pointer.has_value())
                return InterpreterResult::error(InterpreterError::RuntimeError);

            // this is already checked in binder
            if (not pointer.get_value().type.is_pointer())
                return InterpreterResult::error(InterpreterError::ReachedUnreachableError);
            
            if (not std::holds_alternative<Type::PrimitiveValue>(pointer.get_value().value.value))
                return InterpreterResult::error(InterpreterError::DereferenceError);


            auto primitive_val = std::get<Type::PrimitiveValue>(pointer.get_value().value.value);
            if (not std::holds_alternative<Type::Primitive::PTR>(primitive_val))
                return InterpreterResult::error(InterpreterError::DereferenceError);
            
            auto address_value = std::get<Type::Primitive::PTR>(primitive_val);

            if (state.memory.write_value(address_value, result_to_assign.get_value().value, result_to_assign.get_value().type))
                return result_to_assign;


            return InterpreterError::RuntimeError;
        }
    }
}