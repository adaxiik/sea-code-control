#pragma once
#include "lowering/instruction.hpp"
#include "binding/bound_literal_expression.hpp"

namespace scc
{
    namespace lowering
    {
        struct PushLiteralInstruction : public Instruction
        {
            const binding::BoundLiteralExpression& bound_literal_expression;
            PushLiteralInstruction(const binding::BoundLiteralExpression& bound_literal_expression);
            ~PushLiteralInstruction() = default;
            virtual InterpreterResult execute(InterpreterState &state) override;
            virtual InstructionKind instruction_kind() const override;
        };
    }
} 
