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
            return InterpreterError::None;
        }
    }
}