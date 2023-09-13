#pragma once
#include "lowering/instruction.hpp"

namespace scc
{
    namespace lowering
    {
        struct DropInstruction : public Instruction
        {
            const size_t count;
            DropInstruction(size_t count = 1) : count(count) {}
            ~DropInstruction() = default;
            virtual InterpreterResult execute(InterpreterState &state) override;
            virtual InstructionKind instruction_kind() const override;
        };
    }
} 
