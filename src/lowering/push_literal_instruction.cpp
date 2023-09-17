#include "lowering/push_literal_instruction.hpp"

namespace scc
{
    namespace lowering
    {
        InterpreterResult PushLiteralInstruction::execute(InterpreterState &) const
        {
            return InterpreterResult::ok(InterpreterResultValue(value, type));
        }
    }
}