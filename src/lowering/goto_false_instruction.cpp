#include "lowering/goto_false_instruction.hpp"

namespace scc
{
    namespace lowering
    {
        GotoFalseInstruction::GotoFalseInstruction(Label label) : label(label)
        {
        }

        InterpreterResult GotoFalseInstruction::execute(InterpreterState &state) const
        {
            if (state.result_stack.empty())
                return InterpreterError::RuntimeError;
            
            if (state.labels.find(label) == state.labels.end())
                return InterpreterError::RuntimeError;
            
            auto result = state.result_stack.top();
            state.result_stack.pop();

            if (result.get_value().type != Type(Type::PrimitiveType::Bool))
                return InterpreterError::InvalidOperationError;
            
            if (not std::get<bool>(result.get_value().value.primitive_value().value()))
                state.instruction_pointer = state.labels.at(label);

            return InterpreterError::None;
        }
    }
}