#pragma once
#include "interpreter_result.hpp"
#include "interpreter_state.hpp"
namespace scc
{
    namespace lowering
    {
        enum class InstructionKind
        {
            CreateVariable,
            BinaryOp,
            PushLiteral,
            Cast,
            PushScope,
            PopScope,
            COUNT
        };

        class Instruction
        {
        public:
            virtual ~Instruction() = default;
            virtual InterpreterResult execute(InterpreterState &state) = 0;
            virtual InstructionKind instruction_kind() const = 0;
        };
    }
}