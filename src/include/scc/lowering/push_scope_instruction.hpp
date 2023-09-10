#pragma once
#include "lowering/instruction.hpp"

namespace scc
{
    namespace lowering
    {
        class PushScopeInstruction : public Instruction
        {
        public:
            PushScopeInstruction() = default;
            ~PushScopeInstruction() = default;
            virtual InterpreterResult execute(InterpreterState &state) override;
            virtual InstructionKind instruction_kind() const override;
        };
    }
} 
