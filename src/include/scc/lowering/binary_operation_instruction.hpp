#pragma once
#include "interpreter_result.hpp"
#include "interpreter_state.hpp"
#include "binding/bound_binary_expression.hpp"


namespace scc
{
    namespace lowering
    {
        struct BinaryOperationInstruction
        {
            using OperatorKind = binding::BoundBinaryExpression::OperatorKind;
            const OperatorKind operator_kind;
            BinaryOperationInstruction(OperatorKind operator_kind) : operator_kind(operator_kind) {}
            InterpreterResult execute(InterpreterState &state) const;
        };
    }
} 
