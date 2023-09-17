#include "lowering/goto_instruction.hpp"

namespace scc
{
    namespace lowering
    {
        GotoInstruction::GotoInstruction(Label label) : label(label)
        {
        }
        InterpreterResult GotoInstruction::execute(InterpreterState &state) const
        {
            auto label_it = state.labels.find(label);
            
            if (label_it == state.labels.end())
                return InterpreterError::RuntimeError;

            state.instruction_pointer = label_it->second;

            return InterpreterError::None;
        }
    }
}