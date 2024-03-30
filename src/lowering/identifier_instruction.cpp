#include "lowering/identifier_instruction.hpp"

namespace scc
{
    namespace lowering
    {
        InterpreterResult IdentifierInstruction::execute(InterpreterState &state) const
        {
            auto variable = state.call_stack.scope_stack().get_from_scopestack(identifier);
            if (not variable)
                variable = state.global_scope.get_variable(identifier);

            if (not variable)
                return InterpreterError::VariableDoesntExistError;

            // if (not variable->is_initialized())
                // return InterpreterError::VariableNotInitializedError;


            auto value = variable->get_value(state.memory);
            if (not value)
                return InterpreterError::MemoryError; // or memory error??

            return InterpreterResult::ok(InterpreterResultValue(value.value(), variable->type()));
        }
    }
}