#include "lowering/push_scope_instruction.hpp"

namespace scc
{
    namespace lowering
    {
        InterpreterResult PushScopeInstruction::execute(InterpreterState &state) const
        {
            state.scope_stack.push();
            return InterpreterError::None;
        }
    }
}