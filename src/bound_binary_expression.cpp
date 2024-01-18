#include "binding/bound_binary_expression.hpp"

namespace scc
{
    namespace binding
    {
        
        template <typename OP_RESULT_TYPE_PR, typename OP_RESULT_TYPE_LP, typename OP_RESULT_TYPE_LR, typename LEFT_CAST, typename RIGHT_CAST>
        std::optional<Type> binary_operation_result(size_t left_pointer_depth, size_t right_pointer_depth)
        {
            if (left_pointer_depth > 0)                                                                                                                                          \
            {                                                                                                                                                                      \
                if constexpr (std::is_same_v<typename OP_RESULT_TYPE_PR::type, OperationResult::InvalidOperation>)
                    return std::nullopt;
                else
                {
                    auto result_type = Type::deduce_type<LEFT_CAST>();
                    result_type.modifiers = std::vector<Type::Modifier>(left_pointer_depth, Type::Pointer{});
                    return result_type;
                }
            }
            else if (right_pointer_depth > 0)
            {
                if constexpr (std::is_same_v<typename OP_RESULT_TYPE_LP::type, OperationResult::InvalidOperation>)
                    return std::nullopt;
                else
                {
                    auto result_type = Type::deduce_type<RIGHT_CAST>();
                    result_type.modifiers = std::vector<Type::Modifier>(right_pointer_depth, Type::Pointer{});
                    return result_type;
                }
            }
            else
            {
                if constexpr (std::is_same_v<typename OP_RESULT_TYPE_LR::type, OperationResult::InvalidOperation>)
                    return std::nullopt;
                else
                    return Type::deduce_type<typename OP_RESULT_TYPE_LR::type>();
            }       
        }

        std::optional<Type> BoundBinaryExpression::deduce_type(Type left, Type right, OperatorKind op_kind)
        {
            static_assert(static_cast<int>(OperatorKind::COUNT) == 18, "Update this code");

            static_assert(std::variant_size_v<Type::BaseType> == 2, "Update this code");
            if (std::holds_alternative<Type::StructType>(left.base_type) or std::holds_alternative<Type::StructType>(right.base_type))
                return std::nullopt;


        #define DO_CASTED_OP(STRUCT_NAME, LEFT_CAST, RIGHT_CAST) return binary_operation_result< \
            OperationResult::STRUCT_NAME ## OperationResult <Type::Primitive::PTR, RIGHT_CAST>,  \
            OperationResult::STRUCT_NAME ## OperationResult <LEFT_CAST, Type::Primitive::PTR>,   \
            OperationResult::STRUCT_NAME ## OperationResult <LEFT_CAST, RIGHT_CAST>, LEFT_CAST, RIGHT_CAST>(left.pointer_depth(), right.pointer_depth())

        #define RIGHT_CASE(STRUCT_NAME, LEFT_CAST, TYPE, CTYPE) \
            case Type::PrimitiveType::TYPE:                     \
                DO_CASTED_OP(STRUCT_NAME, LEFT_CAST, CTYPE)

        #define DO_OP_RIGHT(STRUCT_NAME, LEFT_CAST)                                                      \
            do                                                                                           \
            {                                                                                            \
                switch (std::get<Type::PrimitiveType>(right.base_type))                               \
                {                                                                                        \
                    RIGHT_CASE(STRUCT_NAME, LEFT_CAST, Char, char);                                      \
                    RIGHT_CASE(STRUCT_NAME, LEFT_CAST, U8, unsigned char);                               \
                    RIGHT_CASE(STRUCT_NAME, LEFT_CAST, I8, signed char);                                 \
                    RIGHT_CASE(STRUCT_NAME, LEFT_CAST, U32, unsigned int);                               \
                    RIGHT_CASE(STRUCT_NAME, LEFT_CAST, I32, int);                                        \
                    RIGHT_CASE(STRUCT_NAME, LEFT_CAST, U64, unsigned long long);                         \
                    RIGHT_CASE(STRUCT_NAME, LEFT_CAST, I64, long long);                                  \
                    RIGHT_CASE(STRUCT_NAME, LEFT_CAST, F32, float);                                      \
                    RIGHT_CASE(STRUCT_NAME, LEFT_CAST, F64, double);                                     \
                    RIGHT_CASE(STRUCT_NAME, LEFT_CAST, Bool, bool);                                      \
                default:                                                                                 \
                    std::cerr << "UNREACHABLE at " << __FILE__ << ":" << __LINE__ << std::endl;          \
                    return left;                                                                         \
                }                                                                                        \
            } while (0)

        #define LEFT_CASE(STRUCT_NAME, TYPE, CTYPE) \
            case Type::PrimitiveType::TYPE:         \
                DO_OP_RIGHT(STRUCT_NAME, CTYPE)

        #define DO_OP_LEFT(STRUCT_NAME)                                                      \
            do                                                                                  \
            {                                                                                   \
                switch(std::get<Type::PrimitiveType>(left.base_type))                           \
                {                                                                               \
                    LEFT_CASE(STRUCT_NAME, Char, char);                                      \
                    LEFT_CASE(STRUCT_NAME, U8, unsigned char);                               \
                    LEFT_CASE(STRUCT_NAME, I8, signed char);                                 \
                    LEFT_CASE(STRUCT_NAME, U32, unsigned int);                               \
                    LEFT_CASE(STRUCT_NAME, I32, int);                                        \
                    LEFT_CASE(STRUCT_NAME, U64, unsigned long long);                         \
                    LEFT_CASE(STRUCT_NAME, I64, long long);                                  \
                    LEFT_CASE(STRUCT_NAME, F32, float);                                      \
                    LEFT_CASE(STRUCT_NAME, F64, double);                                     \
                    LEFT_CASE(STRUCT_NAME, Bool, bool);                                      \
                default:                                                                        \
                    std::cerr << "UNREACHABLE at " << __FILE__ << ":" << __LINE__ << std::endl; \
                    return left;                                                                \
                }                                                                               \
            } while (0)

            switch (op_kind)
            {
            case OperatorKind::Addition:           DO_OP_LEFT(Addition);
            case OperatorKind::Subtraction:        DO_OP_LEFT(Subtraction);
            case OperatorKind::Multiplication:     DO_OP_LEFT(Multiplication);
            case OperatorKind::Division:           DO_OP_LEFT(Division);
            case OperatorKind::Modulo:             DO_OP_LEFT(Modulo);
            case OperatorKind::BitwiseAnd:         DO_OP_LEFT(BitwiseAnd);
            case OperatorKind::BitwiseOr:          DO_OP_LEFT(BitwiseOr);
            case OperatorKind::BitwiseXor:         DO_OP_LEFT(BitwiseXor);
            case OperatorKind::BitwiseShiftLeft:   DO_OP_LEFT(BitwiseShiftLeft);
            case OperatorKind::BitwiseShiftRight:  DO_OP_LEFT(BitwiseShiftRight);
            case OperatorKind::LogicalAnd:         DO_OP_LEFT(LogicalAnd);
            case OperatorKind::LogicalOr:          DO_OP_LEFT(LogicalOr);
            case OperatorKind::Equals:             DO_OP_LEFT(Equals);
            case OperatorKind::NotEquals:          DO_OP_LEFT(NotEquals);
            case OperatorKind::LessThan:           DO_OP_LEFT(LessThan);
            case OperatorKind::GreaterThan:        DO_OP_LEFT(GreaterThan);
            case OperatorKind::LessThanOrEqual:    DO_OP_LEFT(LessThanOrEqual);
            case OperatorKind::GreaterThanOrEqual: DO_OP_LEFT(GreaterThanOrEqual);
            default:
                // UNREACHABLE
                std::cerr << "UNREACHABLE at " << __FILE__ << ":" << __LINE__ << std::endl;
                return left;
            }

#undef DO_OP_LEFT
#undef LEFT_CASE
#undef DO_OP_RIGHT
#undef RIGHT_CASE
#undef DO_CASTED_OP
        }
    }
}
