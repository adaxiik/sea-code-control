#include "binding/bound_binary_expression.hpp"

namespace scc
{
    namespace binding
    {
        // BoundBinaryExpression::BoundBinaryExpression(std::unique_ptr<BoundExpression> left, std::unique_ptr<BoundExpression> right, OperatorKind op_kind)
        //         : BoundExpression(deduce_type(*left, *right, op_kind)), left(std::move(left)), right(std::move(right)), op_kind(op_kind)
        // {
        // }


        std::optional<Type> BoundBinaryExpression::deduce_type(Type left, Type right, OperatorKind op_kind)
        {
            static_assert(static_cast<int>(OperatorKind::COUNT) == 18, "Update this code");


        #define DO_CASTED_OP(OP, STRUCT_NAME, LEFT_CAST, RIGHT_CAST) do{ \
            if constexpr (std::is_same_v<typename OperationResult::STRUCT_NAME ## OperationResult <LEFT_CAST,RIGHT_CAST>::type, OperationResult::InvalidOperation>) \
            { \
                return std::nullopt; \
            } \
            else \
            { \
                return Type::deduce_type<typename OperationResult::STRUCT_NAME ## OperationResult <LEFT_CAST,RIGHT_CAST>::type>(); \
            } \
         }while(0)

        #define RIGHT_CASE(OP, STRUCT_NAME, LEFT_CAST, TYPE, CTYPE) \
            case Type::Kind::TYPE:                     \
                DO_CASTED_OP(OP, STRUCT_NAME, LEFT_CAST, CTYPE)

        #define DO_OP_RIGHT(OP, STRUCT_NAME, LEFT_CAST)                                                      \
            do                                                                                               \
            {                                                                                                \
                switch (right.kind)                                                                          \
                {                                                                                            \
                    RIGHT_CASE(OP, STRUCT_NAME, LEFT_CAST, Char, char);                                      \
                    RIGHT_CASE(OP, STRUCT_NAME, LEFT_CAST, U8, unsigned char);                               \
                    RIGHT_CASE(OP, STRUCT_NAME, LEFT_CAST, I8, signed char);                                 \
                    RIGHT_CASE(OP, STRUCT_NAME, LEFT_CAST, U32, unsigned int);                               \
                    RIGHT_CASE(OP, STRUCT_NAME, LEFT_CAST, I32, int);                                        \
                    RIGHT_CASE(OP, STRUCT_NAME, LEFT_CAST, U64, unsigned long long);                         \
                    RIGHT_CASE(OP, STRUCT_NAME, LEFT_CAST, I64, long long);                                  \
                    RIGHT_CASE(OP, STRUCT_NAME, LEFT_CAST, F32, float);                                      \
                    RIGHT_CASE(OP, STRUCT_NAME, LEFT_CAST, F64, double);                                     \
                    RIGHT_CASE(OP, STRUCT_NAME, LEFT_CAST, Bool, bool);                                      \
                default:                                                                        \
                    std::cerr << "UNREACHABLE at " << __FILE__ << ":" << __LINE__ << std::endl; \
                    return left;                                                           \
                }                                                                               \
            } while (0)

        #define LEFT_CASE(OP, STRUCT_NAME, TYPE, CTYPE) \
            case Type::Kind::TYPE:         \
                DO_OP_RIGHT(OP, STRUCT_NAME, CTYPE)

        #define DO_OP_LEFT(OP,STRUCT_NAME)                                                      \
            do                                                                                  \
            {                                                                                   \
                switch (left.kind)                                                              \
                {                                                                               \
                    LEFT_CASE(OP,STRUCT_NAME, Char, char);                                      \
                    LEFT_CASE(OP,STRUCT_NAME, U8, unsigned char);                               \
                    LEFT_CASE(OP,STRUCT_NAME, I8, signed char);                                 \
                    LEFT_CASE(OP,STRUCT_NAME, U32, unsigned int);                               \
                    LEFT_CASE(OP,STRUCT_NAME, I32, int);                                        \
                    LEFT_CASE(OP,STRUCT_NAME, U64, unsigned long long);                         \
                    LEFT_CASE(OP,STRUCT_NAME, I64, long long);                                  \
                    LEFT_CASE(OP,STRUCT_NAME, F32, float);                                      \
                    LEFT_CASE(OP,STRUCT_NAME, F64, double);                                     \
                    LEFT_CASE(OP,STRUCT_NAME, Bool, bool);                                      \
                default:                                                                        \
                    std::cerr << "UNREACHABLE at " << __FILE__ << ":" << __LINE__ << std::endl; \
                    return left;                                                                \
                }                                                                               \
            } while (0)

            switch (op_kind)
            {
            case OperatorKind::Addition:           DO_OP_LEFT(addition,Addition);
            case OperatorKind::Subtraction:        DO_OP_LEFT(subtraction,Subtraction);
            case OperatorKind::Multiplication:     DO_OP_LEFT(multiplication,Multiplication);
            case OperatorKind::Division:           DO_OP_LEFT(division,Division);
            case OperatorKind::Modulo:             DO_OP_LEFT(modulo,Modulo);
            case OperatorKind::BitwiseAnd:         DO_OP_LEFT(bitwiseand,BitwiseAnd);
            case OperatorKind::BitwiseOr:          DO_OP_LEFT(bitwiseor,BitwiseOr);
            case OperatorKind::BitwiseXor:         DO_OP_LEFT(bitwisexor,BitwiseXor);
            case OperatorKind::BitwiseShiftLeft:   DO_OP_LEFT(bitwiseshiftleft,BitwiseShiftLeft);
            case OperatorKind::BitwiseShiftRight:  DO_OP_LEFT(bitwiseshiftright,BitwiseShiftRight);
            case OperatorKind::LogicalAnd:         DO_OP_LEFT(logicaland,LogicalAnd);
            case OperatorKind::LogicalOr:          DO_OP_LEFT(logicalor,LogicalOr);
            case OperatorKind::Equals:             DO_OP_LEFT(equals,Equals);
            case OperatorKind::NotEquals:          DO_OP_LEFT(notequals,NotEquals);
            case OperatorKind::LessThan:           DO_OP_LEFT(lessthan,LessThan);
            case OperatorKind::GreaterThan:        DO_OP_LEFT(greaterthan,GreaterThan);
            case OperatorKind::LessThanOrEqual:    DO_OP_LEFT(lessthanorequal,LessThanOrEqual);
            case OperatorKind::GreaterThanOrEqual: DO_OP_LEFT(greaterthanorequal,GreaterThanOrEqual);
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
