#include "lowering/reference_instruction.hpp"

namespace scc
{
    namespace lowering
    {
        InterpreterResult ReferenceInstruction::execute(InterpreterState &state) const
        {
            auto variable = state.call_stack.scope_stack().get_from_scopestack(identifier);
            if (not variable)
                variable = state.global_scope.get_variable(identifier);

            if (not variable)
                return InterpreterError::VariableDoesntExistError;

            // if (not variable->is_initialized())
            //     return InterpreterError::VariableNotInitializedError;


            Type type = variable->type();
            type.modifiers.push_back(Type::Pointer{});

            return InterpreterResult::ok(InterpreterResultValue(variable->address(), type));
        }
    }
}