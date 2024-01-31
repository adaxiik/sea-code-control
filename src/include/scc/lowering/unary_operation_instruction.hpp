#pragma once
#include "interpreter_result.hpp"
#include "interpreter_state.hpp"
#include "binding/bound_unary_expression.hpp"


namespace scc
{
    namespace lowering
    {
        struct UnaryOperationInstruction
        {
            using OperatorKind = binding::BoundUnaryExpression::OperatorKind;
            const OperatorKind operator_kind;
            UnaryOperationInstruction(OperatorKind operator_kind) : operator_kind(operator_kind) {}
            InterpreterResult execute(InterpreterState &state) const;
        };
    }
}
