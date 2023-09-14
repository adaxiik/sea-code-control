#pragma once
#include "interpreter_result.hpp"
#include <variant>
#include <type_traits>
#include "lowering/binary_operation_instruction.hpp"
#include "lowering/cast_instruction.hpp"
#include "lowering/drop_instruction.hpp"
#include "lowering/pop_scope_instruction.hpp"
#include "lowering/push_literal_instruction.hpp"
#include "lowering/push_scope_instruction.hpp"

namespace scc
{
    namespace lowering
    {
        using Instruction = std::variant<
            BinaryOperationInstruction,
            CastInstruction,
            DropInstruction,
            PopScopeInstruction,
            PushLiteralInstruction,
            PushScopeInstruction>;

        constexpr auto InstructionCount = std::variant_size_v<Instruction>;

        struct InstructionExecuter
        {
            InterpreterState& state;
            InstructionExecuter(InterpreterState& state): state(state){}

            template <typename T>
            InterpreterResult operator()(const T& instruction)
            {
                return instruction.execute(state);
            }
        };
    }
}