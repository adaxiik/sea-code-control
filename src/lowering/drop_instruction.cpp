#include "lowering/drop_instruction.hpp"

namespace scc
{
    namespace lowering
    {
        InterpreterResult DropInstruction::execute(InterpreterState &state) const
        {
            for (size_t i = 0; i < count; i++)
                state.result_stack.pop();
                
            return InterpreterError::None;
        }
    }
}