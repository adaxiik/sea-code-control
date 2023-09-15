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
            return InterpreterError::None;
        }
    }
}