#pragma once
#include "lowering/instruction.hpp"
#include "binding/bound_binary_expression.hpp"

namespace scc
{
    namespace lowering
    {
        struct BinaryOperationInstruction : public Instruction
        {
            using OperatorKind = binding::BoundBinaryExpression::OperatorKind;
            const OperatorKind operator_kind;
            BinaryOperationInstruction(OperatorKind operator_kind) : operator_kind(operator_kind) {}
            ~BinaryOperationInstruction() = default;
            virtual InterpreterResult execute(InterpreterState &state) override;
            virtual InstructionKind instruction_kind() const override;
        };
    }
} 
