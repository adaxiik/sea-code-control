#include "lowering/cast_instruction.hpp"
namespace scc
{
    namespace lowering
    {
        InterpreterResult CastInstruction::execute(InterpreterState &state) const
        {
            if (state.result_stack.empty())
                return InterpreterError::RuntimeError;

            InterpreterResult result{state.result_stack.top()};
            state.result_stack.pop();

            if (result.is_error())
                return result;

            auto target_type{type};

            // TODOOO: casting for structs
            if (not target_type.is_primitive())
                return InterpreterResult::error(InterpreterError::InvalidOperationError);

            // (int)(int*) 
            if (result.get_value().type.is_pointer() and not target_type.is_pointer())
                return InterpreterResult::error(InterpreterError::InvalidOperationError);

            if (target_type == result.get_value().type)
                return result;

            if (target_type.is_pointer())
            {
                if (result.get_value().type.is_pointer())
                    return InterpreterResult::ok(InterpreterResultValue(static_cast<Type::Primitive::U64>(std::get<Type::Primitive::U64>(result.get_value().value.primitive_value().value())), target_type));

                switch (result.get_value().type.primitive_type().value_or(Type::PrimitiveType::COUNT))
                {
                    case Type::PrimitiveType::Char:
                        return InterpreterResult::ok(InterpreterResultValue(static_cast<Type::Primitive::U64>(std::get<Type::Primitive::Char>(result.get_value().value.primitive_value().value())), target_type));
                    case Type::PrimitiveType::U8:
                        return InterpreterResult::ok(InterpreterResultValue(static_cast<Type::Primitive::U64>(std::get<Type::Primitive::U8>(result.get_value().value.primitive_value().value())), target_type));
                    case Type::PrimitiveType::I8:
                        return InterpreterResult::ok(InterpreterResultValue(static_cast<Type::Primitive::U64>(std::get<Type::Primitive::I8>(result.get_value().value.primitive_value().value())), target_type));
                    case Type::PrimitiveType::U16:
                        return InterpreterResult::ok(InterpreterResultValue(static_cast<Type::Primitive::U64>(std::get<Type::Primitive::U16>(result.get_value().value.primitive_value().value())), target_type));
                    case Type::PrimitiveType::I16:
                        return InterpreterResult::ok(InterpreterResultValue(static_cast<Type::Primitive::U64>(std::get<Type::Primitive::I16>(result.get_value().value.primitive_value().value())), target_type));
                    case Type::PrimitiveType::U32:
                        return InterpreterResult::ok(InterpreterResultValue(static_cast<Type::Primitive::U64>(std::get<Type::Primitive::U32>(result.get_value().value.primitive_value().value())), target_type));
                    case Type::PrimitiveType::I32:
                        return InterpreterResult::ok(InterpreterResultValue(static_cast<Type::Primitive::U64>(std::get<Type::Primitive::I32>(result.get_value().value.primitive_value().value())), target_type));
                    case Type::PrimitiveType::U64:
                        return InterpreterResult::ok(InterpreterResultValue(static_cast<Type::Primitive::U64>(std::get<Type::Primitive::U64>(result.get_value().value.primitive_value().value())), target_type));
                    case Type::PrimitiveType::I64:
                        return InterpreterResult::ok(InterpreterResultValue(static_cast<Type::Primitive::U64>(std::get<Type::Primitive::I64>(result.get_value().value.primitive_value().value())), target_type));
                    case Type::PrimitiveType::F32:
                        return InterpreterResult::ok(InterpreterResultValue(static_cast<Type::Primitive::U64>(std::get<Type::Primitive::F32>(result.get_value().value.primitive_value().value())), target_type));
                    case Type::PrimitiveType::F64:
                        return InterpreterResult::ok(InterpreterResultValue(static_cast<Type::Primitive::U64>(std::get<Type::Primitive::F64>(result.get_value().value.primitive_value().value())), target_type));
                    case Type::PrimitiveType::Bool:
                        return InterpreterResult::ok(InterpreterResultValue(static_cast<Type::Primitive::U64>(std::get<Type::Primitive::Bool>(result.get_value().value.primitive_value().value())), target_type));
                    case Type::PrimitiveType::Void:
                        return InterpreterResult::ok(InterpreterResultValue(static_cast<Type::Primitive::U64>(0), target_type));
                    default:
                        return InterpreterResult::error(InterpreterError::ReachedUnreachableError);
                }
            }

            static_assert(static_cast<int>(Type::PrimitiveType::COUNT) == 13, "Update this code");


            #define CAST_CASE(KIND_TYPE, TARGET_TYPE) case Type::PrimitiveType::KIND_TYPE: \
                return InterpreterResult::ok(InterpreterResultValue(static_cast<TARGET_TYPE>(std::get<Type::Primitive::KIND_TYPE>(result.get_value().value.primitive_value().value()))))
            
            #define CAST_ORIGINAL(TARGET_TYPE) do{ \
                switch(result.get_value().type.primitive_type().value_or(Type::PrimitiveType::COUNT)) \
                { \
                    CAST_CASE(Char, TARGET_TYPE); \
                    CAST_CASE(U8, TARGET_TYPE); \
                    CAST_CASE(I8, TARGET_TYPE); \
                    CAST_CASE(U16, TARGET_TYPE); \
                    CAST_CASE(I16, TARGET_TYPE); \
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

            switch(target_type.primitive_type().value_or(Type::PrimitiveType::COUNT))
            {
                case Type::PrimitiveType::Char: CAST_ORIGINAL(Type::Primitive::Char);
                case Type::PrimitiveType::U8: CAST_ORIGINAL(Type::Primitive::U8);
                case Type::PrimitiveType::I8: CAST_ORIGINAL(Type::Primitive::I8);
                case Type::PrimitiveType::U16: CAST_ORIGINAL(Type::Primitive::U16);
                case Type::PrimitiveType::I16: CAST_ORIGINAL(Type::Primitive::I16);
                case Type::PrimitiveType::U32: CAST_ORIGINAL(Type::Primitive::U32);
                case Type::PrimitiveType::I32: CAST_ORIGINAL(Type::Primitive::I32);
                case Type::PrimitiveType::U64: CAST_ORIGINAL(Type::Primitive::U64);
                case Type::PrimitiveType::I64: CAST_ORIGINAL(Type::Primitive::I64);
                case Type::PrimitiveType::F32: CAST_ORIGINAL(Type::Primitive::F32);
                case Type::PrimitiveType::F64: CAST_ORIGINAL(Type::Primitive::F64);
                case Type::PrimitiveType::Bool: CAST_ORIGINAL(Type::Primitive::Bool);
                case Type::PrimitiveType::Void: 
                    return InterpreterResult::error(InterpreterError::InvalidOperationError);
                default:
                    return InterpreterResult::error(InterpreterError::ReachedUnreachableError); 
            }
            #undef CAST_CASE
            #undef CAST_ORIGINAL
        }
    }
}