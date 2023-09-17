#include "lowering/push_scope_instruction.hpp"

namespace scc
{
    namespace lowering
    {
        InterpreterResult PushScopeInstruction::execute(InterpreterState &state) const
        {
            state.call_stack.push_scope();
            return InterpreterError::None;
        }
    }
}