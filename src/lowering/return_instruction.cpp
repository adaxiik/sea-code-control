#include "lowering/return_instruction.hpp"

namespace scc
{
    namespace lowering
    {
        InterpreterResult ReturnInstruction::execute(InterpreterState &state) const
        {
            state.instruction_pointer = state.call_stack.pop();

            if (!has_return_expression)
                return InterpreterError::None;

            if (state.result_stack.empty())
                return InterpreterError::RuntimeError;
            
            InterpreterResult result = state.result_stack.top();
            state.result_stack.pop();
            return result;
        }
    }
}