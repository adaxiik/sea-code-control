#include "binding/bound_unary_expression.hpp"
#include "cpp_compiler.hpp"

namespace scc
{
    namespace binding
    {
        template <typename T>
        static Type binary_not_operation()
        {
            SCC_SUPPRESS_WARNING_PUSH_BOOL_OPERATION;
            return Type::deduce_type<decltype(~std::declval<T>())>();
            SCC_SUPPRESS_WARNING_POP_BOOL_OPERATION;
        }

        std::optional<Type> BoundUnaryExpression::deduce_type(Type expression_type, OperatorKind op_kind)
        {
            static_assert(static_cast<int>(OperatorKind::COUNT) == 2, "Update this code");

            if (expression_type.is_pointer())
            {
                if (op_kind == OperatorKind::BinaryNot)
                    return std::nullopt;
                else
                    return Type(Type::PrimitiveType::Bool);
            }

            static_assert(std::variant_size_v<Type::BaseType> == 2, "Update this code");
            if (std::holds_alternative<Type::StructType>(expression_type.base_type))
                return std::nullopt;

            if (expression_type.primitive_type() == Type::PrimitiveType::Void)
                return std::nullopt;

            if (op_kind == OperatorKind::LogicalNot)
            {
                return Type(Type::PrimitiveType::Bool);
            }

            if (op_kind == OperatorKind::BinaryNot and
                    (
                        expression_type == Type(Type::PrimitiveType::F32)
                     or expression_type == Type(Type::PrimitiveType::F64)
                    )
                )
            {
                return std::nullopt;
            }

            switch (std::get<Type::PrimitiveType>(expression_type.base_type))
            {
                case Type::PrimitiveType::Char: return binary_not_operation<Type::Primitive::Char>();
                case Type::PrimitiveType::U8: return binary_not_operation<Type::Primitive::U8>();
                case Type::PrimitiveType::I8: return binary_not_operation<Type::Primitive::I8>();
                case Type::PrimitiveType::U16: return binary_not_operation<Type::Primitive::U16>();
                case Type::PrimitiveType::I16: return binary_not_operation<Type::Primitive::I16>();
                case Type::PrimitiveType::U32: return binary_not_operation<Type::Primitive::U32>();
                case Type::PrimitiveType::I32: return binary_not_operation<Type::Primitive::I32>();
                case Type::PrimitiveType::U64: return binary_not_operation<Type::Primitive::U64>();
                case Type::PrimitiveType::I64: return binary_not_operation<Type::Primitive::I64>();
                // case Type::PrimitiveType::F32: return binary_not_operation<Type::Primitive::F32>();
                // case Type::PrimitiveType::F64: return binary_not_operation<Type::Primitive::F64>();
                case Type::PrimitiveType::Bool: return binary_not_operation<Type::Primitive::Bool>();
                default:
                    std::cerr << "UNREACHABLE at " << __FILE__ << ":" << __LINE__ << std::endl;
                    return std::nullopt;
            }
            return std::nullopt;
        }
    }
}
