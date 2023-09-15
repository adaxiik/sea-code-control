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
            return InterpreterError::None;
        }
    }
}