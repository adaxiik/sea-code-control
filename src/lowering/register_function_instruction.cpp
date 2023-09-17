#include "lowering/register_function_instruction.hpp"

namespace scc
{
    namespace lowering
    {
        InterpreterResult RegisterFunctionInstruction::execute(InterpreterState &) const
        {
            // this instruction actually does nothing.
            // interpreter will loop over the all instructions before execution and register all functions.
            return InterpreterError::None;
        }
    }
}