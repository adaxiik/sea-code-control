#include "lowering/push_scope_instruction.hpp"

namespace scc
{
    namespace lowering
    {
        InterpreterResult PushScopeInstruction::execute(InterpreterState &state)
        {
            state.scope_stack.push();
            return InterpreterError::None;
        }

        InstructionKind PushScopeInstruction::instruction_kind() const
        {
            return InstructionKind::PushScope;
        }
    }
}