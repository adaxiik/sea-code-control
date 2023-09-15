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
#include "lowering/create_value_variable_instruction.hpp"
#include "lowering/identifier_instruction.hpp"
#include "lowering/assignment_instruction.hpp"
#include "lowering/goto_true_instruction.hpp"
#include "lowering/goto_false_instruction.hpp"
#include "lowering/goto_instruction.hpp"
#include "lowering/label_instruction.hpp"

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
            PushScopeInstruction,
            CreateValueVariableInstruction,
            IdentifierInstruction,
            AssignmentInstruction,
            GotoTrueInstruction,
            GotoFalseInstruction,
            GotoInstruction,
            LabelInstruction>;

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