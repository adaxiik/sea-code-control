#include "lowering/pop_scope_instruction.hpp"

namespace scc
{
    namespace lowering
    {
        InterpreterResult PopScopeInstruction::execute(InterpreterState &state)
        {
            state.scope_stack.pop();
            return InterpreterError::None;
        }

        InstructionKind PopScopeInstruction::instruction_kind() const
        {
            return InstructionKind::PopScope;
        }
    }
}