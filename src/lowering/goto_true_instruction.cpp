#include "lowering/goto_true_instruction.hpp"

namespace scc
{
    namespace lowering
    {
        GotoTrueInstruction::GotoTrueInstruction(Label label) : label(label)
        {
        }
        
        InterpreterResult GotoTrueInstruction::execute(InterpreterState &state) const
        {
            if (state.result_stack.empty())
                return InterpreterError::RuntimeError;
            
            if (state.labels.find(label) == state.labels.end())
                return InterpreterError::RuntimeError;
            
            auto result = state.result_stack.top();
            state.result_stack.pop();
            
            if (std::get<bool>(result.get_value().value))
                state.instruction_pointer = state.labels.at(label);

            return InterpreterError::None;
        }
    }
}