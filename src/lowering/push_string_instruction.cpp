#include "lowering/push_string_instruction.hpp"

namespace scc
{
    namespace lowering
    {
        InterpreterResult PushStringInstruction::execute(InterpreterState &state) const
        {
            if (state.read_only_string_literals.find(value) == state.read_only_string_literals.end())
            {
                state.read_only_string_literals[value] = state.memory.allocate(value.size() + 1, true);
                state.memory.write_buffer_unsafe(state.read_only_string_literals[value], value.c_str(), value.size() + 1);
            }
            Type t = Type(Type::PrimitiveType::Char, {Type::Pointer{}});
            return InterpreterResult::ok(
                InterpreterResultValue(
                    Type::Value(std::move(t),
                    static_cast<Type::Primitive::U64>(state.read_only_string_literals.at(value)))
                )
            );
        }
    }
}