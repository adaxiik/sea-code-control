#pragma once
#include "lowering/instruction.hpp"

namespace scc
{
    namespace lowering
    {
        class PopScopeInstruction : public Instruction
        {
        public:
            PopScopeInstruction() = default;
            ~PopScopeInstruction() = default;
            virtual InterpreterResult execute(InterpreterState &state) override;
            virtual InstructionKind instruction_kind() const override;
        };
    }
} 
