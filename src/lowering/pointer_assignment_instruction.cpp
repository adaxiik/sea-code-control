#include "lowering/pointer_assignment_instruction.hpp"

namespace scc
{
    namespace lowering
    {
        InterpreterResult PointerAssignmentInstruction::execute(InterpreterState &state) const
        {
            if (state.result_stack.size() < 2)
                return InterpreterError::RuntimeError;

            // auto result = state.result_stack.top();
            // state.result_stack.pop();

            // auto variable = state.call_stack.scope_stack().get_from_scopestack(variable_name);
            // if (!variable)
            //     variable = state.global_scope.get_variable(variable_name);

            // if (!variable)
            //     return InterpreterError::VariableDoesntExistError;

            // if (variable->set_value(state.memory, result.get_value().value))
            //     return result;

            return InterpreterError::RuntimeError;
        }
    }
}