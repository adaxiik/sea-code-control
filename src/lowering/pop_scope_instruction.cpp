#include "lowering/pop_scope_instruction.hpp"

namespace scc
{
    namespace lowering
    {
        InterpreterResult PopScopeInstruction::execute(InterpreterState &state) const
        {
            state.call_stack.pop_scope();
            return InterpreterError::None;
        }
    }
}