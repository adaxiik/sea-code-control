#include "lowering/drop_instruction.hpp"

namespace scc
{
    namespace lowering
    {
        InterpreterResult DropInstruction::execute(InterpreterState &state) const
        {
            if (state.result_stack.size() < count)
                return InterpreterError::NotEnoughValuesToDropError;

            for (size_t i = 0; i < count; i++)
                state.result_stack.pop();
                
            return InterpreterError::None;
        }
    }
}