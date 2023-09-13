#pragma once
#include "lowering/instruction.hpp"
#include "binding/bound_cast_expression.hpp"

namespace scc
{
    namespace lowering
    {
        struct CastInstruction : public Instruction
        {
            const Type type;
            CastInstruction(Type type) : type(type) {}
            ~CastInstruction() = default;
            virtual InterpreterResult execute(InterpreterState &state) override;
            virtual InstructionKind instruction_kind() const override;
        };
    }
} 
