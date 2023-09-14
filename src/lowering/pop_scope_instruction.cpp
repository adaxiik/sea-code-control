#include "lowering/pop_scope_instruction.hpp"

namespace scc
{
    namespace lowering
    {
        InterpreterResult PopScopeInstruction::execute(InterpreterState &state) const
        {
            state.scope_stack.pop();
            return InterpreterError::None;
        }
    }
}