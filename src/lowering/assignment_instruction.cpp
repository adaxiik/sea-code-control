#include "lowering/assignment_instruction.hpp"

namespace scc
{
    namespace lowering
    {
        AssignmentInstruction::AssignmentInstruction(const std::string &name) : variable_name(name) {}

        InterpreterResult AssignmentInstruction::execute(InterpreterState &state) const
        {
            if (state.result_stack.size() < 1)
                return InterpreterError::RuntimeError;

            auto result = state.result_stack.top();
            state.result_stack.pop();

            auto variable = state.scope_stack.get_from_scopestack(variable_name);
            if (!variable)
                return InterpreterError::VariableDoesntExistError;

            if (variable->set_value(state.memory, result.get_value().value))
                return result;

            return InterpreterError::RuntimeError;
        }
    }
}