#include "lowering/push_literal_instruction.hpp"

namespace scc
{
    namespace lowering
    {
        PushLiteralInstruction::PushLiteralInstruction(const binding::BoundLiteralExpression& bound_literal_expression)
            : bound_literal_expression(bound_literal_expression)
        {}

        InterpreterResult PushLiteralInstruction::execute(InterpreterState &)
        {
            return InterpreterResult::ok(InterpreterResultValue(bound_literal_expression.value, bound_literal_expression.type));
        }

        InstructionKind PushLiteralInstruction::instruction_kind() const
        {
            return InstructionKind::PushLiteral;
        }
    }
}