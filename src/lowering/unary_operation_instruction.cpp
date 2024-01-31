#include "lowering/unary_operation_instruction.hpp"
#include "cpp_compiler.hpp"
namespace scc
{
    namespace lowering
    {

        static InterpreterResult logical_not_op(const InterpreterResultValue& value)
        {
            switch (value.type.primitive_type().value_or(Type::PrimitiveType::COUNT))
            {
                case Type::PrimitiveType::Bool: return InterpreterResult(not std::get<Type::Primitive::Bool>(value.value.primitive_value().value()));
                case Type::PrimitiveType::Char: return InterpreterResult(not std::get<Type::Primitive::Char>(value.value.primitive_value().value()));
                case Type::PrimitiveType::U8: return InterpreterResult(not std::get<Type::Primitive::U8>(value.value.primitive_value().value()));
                case Type::PrimitiveType::I8: return InterpreterResult(not std::get<Type::Primitive::I8>(value.value.primitive_value().value()));
                case Type::PrimitiveType::U16: return InterpreterResult(not std::get<Type::Primitive::U16>(value.value.primitive_value().value()));
                case Type::PrimitiveType::I16: return InterpreterResult(not std::get<Type::Primitive::I16>(value.value.primitive_value().value()));
                case Type::PrimitiveType::U32: return InterpreterResult(not std::get<Type::Primitive::U32>(value.value.primitive_value().value()));
                case Type::PrimitiveType::I32: return InterpreterResult(not std::get<Type::Primitive::I32>(value.value.primitive_value().value()));
                case Type::PrimitiveType::U64: return InterpreterResult(not std::get<Type::Primitive::U64>(value.value.primitive_value().value()));
                case Type::PrimitiveType::I64: return InterpreterResult(not std::get<Type::Primitive::I64>(value.value.primitive_value().value()));
                case Type::PrimitiveType::F32: return InterpreterResult(not std::get<Type::Primitive::F32>(value.value.primitive_value().value()));
                case Type::PrimitiveType::F64: return InterpreterResult(not std::get<Type::Primitive::F64>(value.value.primitive_value().value()));
                default: return InterpreterError::RuntimeError;
            }
        }



        InterpreterResult UnaryOperationInstruction::execute(InterpreterState &state) const
        {
            if (state.result_stack.empty())
                return InterpreterError::RuntimeError;

            InterpreterResult result{state.result_stack.top()};
            state.result_stack.pop();

            static_assert(static_cast<int>(OperatorKind::COUNT) == 2);

            auto value = result.get_value();
            if (value.type.is_pointer())
            {
                if (operator_kind == OperatorKind::BinaryNot)
                    return InterpreterError::ReachedUnreachableError; // handled by the binder

                if (operator_kind == OperatorKind::LogicalNot)
                {
                    Type::Primitive::U64 v = std::get<Type::Primitive::U64>(value.value.primitive_value().value());
                    return InterpreterResult(static_cast<bool>(not v));
                }
            }
            static_assert(std::variant_size_v<Type::BaseType> == 2);
            if (std::holds_alternative<Type::StructType>(value.type.base_type))
                return InterpreterError::ReachedUnreachableError; // handled by the binder as well

            if (value.type.primitive_type() == Type::PrimitiveType::Void)
                return InterpreterError::ReachedUnreachableError;



            if (operator_kind == OperatorKind::LogicalNot)
                return logical_not_op(value);

            if (operator_kind == OperatorKind::BinaryNot and
                    (
                        value.type == Type(Type::PrimitiveType::F32)
                     or value.type == Type(Type::PrimitiveType::F64)
                    )
                )
            {
                return InterpreterError::ReachedUnreachableError;
            }

            switch (std::get<Type::PrimitiveType>(value.type.base_type))
            {
                case Type::PrimitiveType::Char: return InterpreterResult(~std::get<Type::Primitive::Char>(value.value.primitive_value().value()));
                case Type::PrimitiveType::U8: return InterpreterResult(~std::get<Type::Primitive::U8>(value.value.primitive_value().value()));
                case Type::PrimitiveType::I8: return InterpreterResult(~std::get<Type::Primitive::I8>(value.value.primitive_value().value()));
                case Type::PrimitiveType::U16: return InterpreterResult(~std::get<Type::Primitive::U16>(value.value.primitive_value().value()));
                case Type::PrimitiveType::I16: return InterpreterResult(~std::get<Type::Primitive::I16>(value.value.primitive_value().value()));
                case Type::PrimitiveType::U32: return InterpreterResult(~std::get<Type::Primitive::U32>(value.value.primitive_value().value()));
                case Type::PrimitiveType::I32: return InterpreterResult(~std::get<Type::Primitive::I32>(value.value.primitive_value().value()));
                case Type::PrimitiveType::U64: return InterpreterResult(~std::get<Type::Primitive::U64>(value.value.primitive_value().value()));
                case Type::PrimitiveType::I64: return InterpreterResult(~std::get<Type::Primitive::I64>(value.value.primitive_value().value()));
                // case Type::PrimitiveType::F32: return InterpreterResult(~std::get<Type::Primitive::F32>(value.value.primitive_value().value()));
                // case Type::PrimitiveType::F64: return InterpreterResult(~std::get<Type::Primitive::F64>(value.value.primitive_value().value()));
                SCC_SUPPRESS_WARNING_PUSH_BOOL_OPERATION;
                case Type::PrimitiveType::Bool: return InterpreterResult(~std::get<Type::Primitive::Bool>(value.value.primitive_value().value()));
                SCC_SUPPRESS_WARNING_POP_BOOL_OPERATION;
                default:
                    std::cerr << "UNREACHABLE at " << __FILE__ << ":" << __LINE__ << std::endl;
                    return InterpreterError::RuntimeError;
            }

            return InterpreterError::RuntimeError;
        }
    }
}