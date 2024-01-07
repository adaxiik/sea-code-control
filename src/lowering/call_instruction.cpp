#include "lowering/call_instruction.hpp"

namespace scc
{
    namespace lowering
    {
        InterpreterResult CallInstruction::execute(InterpreterState &state) const
        {
            if (state.functions.find(function_name) == state.functions.end())
                return InterpreterError::FunctionNotDefinedError;

            state.call_stack.push(state.instruction_pointer, function_name);
            state.instruction_pointer = state.functions.at(function_name);

            return InterpreterError::None;
        }
    }
}