#include "lowering/identifier_instruction.hpp"

namespace scc
{
    namespace lowering
    {
        InterpreterResult IdentifierInstruction::execute(InterpreterState &state) const
        {
            auto variable = state.scope_stack.get_from_scopestack(identifier);
            if (!variable)
                return InterpreterError::VariableDoesntExistError;

            if (!variable->is_initialized())
                return InterpreterError::VariableNotInitializedError;
        
        
            auto value = variable->get_value(state.memory, variable->type());
            if (!value)
                return InterpreterError::VariableNotInitializedError; // or memory error??

            return InterpreterResult::ok(InterpreterResultValue(value.value()));
        }
    }
}