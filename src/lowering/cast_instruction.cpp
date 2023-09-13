#include "lowering/cast_instruction.hpp"

namespace scc
{
    namespace lowering
    {
        InterpreterResult CastInstruction::execute(InterpreterState &state)
        {
            InterpreterResult result{state.result_stack.top()};
            state.result_stack.pop();

            if (result.is_error())
                return result;

            auto target_type{type};


            if (target_type == result.get_value().type)
                return result;

            if (target_type.pointer_depth > 0)
            {
                switch (result.get_value().type.kind)
                {
                    case Type::Kind::Char:
                        return InterpreterResult::ok(InterpreterResultValue(static_cast<Type::Primitive::U64>(std::get<Type::Primitive::Char>(result.get_value().value)), target_type));
                    case Type::Kind::U8:
                        return InterpreterResult::ok(InterpreterResultValue(static_cast<Type::Primitive::U64>(std::get<Type::Primitive::U8>(result.get_value().value)), target_type));
                    case Type::Kind::I8:
                        return InterpreterResult::ok(InterpreterResultValue(static_cast<Type::Primitive::U64>(std::get<Type::Primitive::I8>(result.get_value().value)), target_type));
                    case Type::Kind::U16:
                        return InterpreterResult::ok(InterpreterResultValue(static_cast<Type::Primitive::U64>(std::get<Type::Primitive::U16>(result.get_value().value)), target_type));
                    case Type::Kind::I16:
                        return InterpreterResult::ok(InterpreterResultValue(static_cast<Type::Primitive::U64>(std::get<Type::Primitive::I16>(result.get_value().value)), target_type));
                    case Type::Kind::U32:
                        return InterpreterResult::ok(InterpreterResultValue(static_cast<Type::Primitive::U64>(std::get<Type::Primitive::U32>(result.get_value().value)), target_type));
                    case Type::Kind::I32:
                        return InterpreterResult::ok(InterpreterResultValue(static_cast<Type::Primitive::U64>(std::get<Type::Primitive::I32>(result.get_value().value)), target_type));
                    case Type::Kind::U64:
                        return InterpreterResult::ok(InterpreterResultValue(static_cast<Type::Primitive::U64>(std::get<Type::Primitive::U64>(result.get_value().value)), target_type));
                    case Type::Kind::I64:
                        return InterpreterResult::ok(InterpreterResultValue(static_cast<Type::Primitive::U64>(std::get<Type::Primitive::U64>(result.get_value().value)), target_type));
                    case Type::Kind::F32:
                        return InterpreterResult::ok(InterpreterResultValue(static_cast<Type::Primitive::U64>(std::get<Type::Primitive::F32>(result.get_value().value)), target_type));
                    case Type::Kind::F64:
                        return InterpreterResult::ok(InterpreterResultValue(static_cast<Type::Primitive::U64>(std::get<Type::Primitive::F64>(result.get_value().value)), target_type));
                    case Type::Kind::Bool:
                        return InterpreterResult::ok(InterpreterResultValue(static_cast<Type::Primitive::U64>(std::get<Type::Primitive::Bool>(result.get_value().value)), target_type));
                    case Type::Kind::Void:
                        return InterpreterResult::ok(InterpreterResultValue(static_cast<Type::Primitive::U64>(0), target_type));
                    default:
                        return InterpreterResult::error(InterpreterError::ReachedUnreachableError);
                }
            }

            static_assert(static_cast<int>(Type::Kind::COUNT) == 13, "Update this code");


            #define CAST_CASE(KIND_TYPE, TARGET_TYPE) case Type::Kind::KIND_TYPE: \
                return InterpreterResult::ok(InterpreterResultValue(static_cast<TARGET_TYPE>(std::get<Type::Primitive::KIND_TYPE>(result.get_value().value))))
            
            #define CAST_ORIGINAL(TARGET_TYPE) do{ \
                switch(result.get_value().type.kind) \
                { \
                    CAST_CASE(Char, TARGET_TYPE); \
                    CAST_CASE(U8, TARGET_TYPE); \
                    CAST_CASE(I8, TARGET_TYPE); \
                    CAST_CASE(U32, TARGET_TYPE); \
                    CAST_CASE(I32, TARGET_TYPE); \
                    CAST_CASE(U64, TARGET_TYPE); \
                    CAST_CASE(I64, TARGET_TYPE); \
                    CAST_CASE(F32, TARGET_TYPE); \
                    CAST_CASE(F64, TARGET_TYPE); \
                    CAST_CASE(Bool, TARGET_TYPE); \
                    default: \
                        return InterpreterResult::error(InterpreterError::ReachedUnreachableError); \
                }}while(0)

            switch(target_type.kind)
            {
                case Type::Kind::Char: CAST_ORIGINAL(Type::Primitive::Char);
                case Type::Kind::U8: CAST_ORIGINAL(Type::Primitive::U8);
                case Type::Kind::I8: CAST_ORIGINAL(Type::Primitive::I8);
                case Type::Kind::U16: CAST_ORIGINAL(Type::Primitive::U16);
                case Type::Kind::I16: CAST_ORIGINAL(Type::Primitive::I16);
                case Type::Kind::U32: CAST_ORIGINAL(Type::Primitive::U32);
                case Type::Kind::I32: CAST_ORIGINAL(Type::Primitive::I32);
                case Type::Kind::U64: CAST_ORIGINAL(Type::Primitive::U64);
                case Type::Kind::I64: CAST_ORIGINAL(Type::Primitive::I64);
                case Type::Kind::F32: CAST_ORIGINAL(Type::Primitive::F32);
                case Type::Kind::F64: CAST_ORIGINAL(Type::Primitive::F64);
                case Type::Kind::Bool: CAST_ORIGINAL(Type::Primitive::Bool);
                case Type::Kind::Void: 
                    return InterpreterResult::error(InterpreterError::InvalidOperationError);
                default:
                    return InterpreterResult::error(InterpreterError::ReachedUnreachableError); 
            }
            #undef CAST_CASE
            #undef CAST_ORIGINAL
        }

        InstructionKind CastInstruction::instruction_kind() const
        {
            return InstructionKind::Cast;
        }
    }
}