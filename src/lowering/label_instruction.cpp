#include "lowering/label_instruction.hpp"

namespace scc
{
    namespace lowering
    {
        LabelInstruction::LabelInstruction(Label label) : label(label)
        {
        }

        InterpreterResult LabelInstruction::execute(InterpreterState &) const
        {
            return InterpreterError::None;
        }
    }
}