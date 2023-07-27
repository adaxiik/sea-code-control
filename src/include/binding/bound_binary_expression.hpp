#pragma once
#include <memory>
#include "binding/bound_expression.hpp"

namespace scc
{
    namespace binding
    {
        struct BoundBinaryExpression : public BoundExpression
        {
            enum class OperatorKind
            {
                Addition,
                Subtraction,

                COUNT
            };

            std::unique_ptr<BoundExpression> left;
            std::unique_ptr<BoundExpression> right;
            OperatorKind op_kind;

            BoundBinaryExpression(std::unique_ptr<BoundExpression> left, std::unique_ptr<BoundExpression> right, OperatorKind op_kind)
                : BoundExpression(deduce_type(*left, *right, op_kind)), left(std::move(left)), right(std::move(right)), op_kind(op_kind)
            {
            }
            // TODOO: move it to cpp file
            static Type deduce_type(const BoundExpression &left, const BoundExpression &right, OperatorKind op_kind)
            {
                static_assert(static_cast<int>(OperatorKind::COUNT) == 2, "Update this code");
                // it is what it is
                #define DO_CASTED_OP(OP, LEFT_CAST, RIGHT_CAST)                                                                        \
                    do                                                                                                                 \
                    {                                                                                                                  \
                        return Type::deduce_type<decltype(std::declval<LEFT_CAST>() + std::declval<RIGHT_CAST>())>();                  \
                    } while (0)

                #define RIGHT_CASE(OP, LEFT_CAST, TYPE, CTYPE)     \
                    case Type::Kind::TYPE:                         \
                        DO_CASTED_OP(OP, LEFT_CAST, CTYPE)

                #define DO_OP_RIGHT(OP, LEFT_CAST)                                                      \
                    do                                                                                  \
                    {                                                                                   \
                        switch (right.type.kind)                                     \
                        {                                                                               \
                            RIGHT_CASE(OP, LEFT_CAST, Char, char);                                      \
                            RIGHT_CASE(OP, LEFT_CAST, U8, unsigned char);                               \
                            RIGHT_CASE(OP, LEFT_CAST, I8, signed char);                                 \
                            RIGHT_CASE(OP, LEFT_CAST, U32, unsigned int);                               \
                            RIGHT_CASE(OP, LEFT_CAST, I32, int);                                        \
                            RIGHT_CASE(OP, LEFT_CAST, U64, unsigned long long);                         \
                            RIGHT_CASE(OP, LEFT_CAST, I64, long long);                                  \
                            RIGHT_CASE(OP, LEFT_CAST, F32, float);                                      \
                            RIGHT_CASE(OP, LEFT_CAST, F64, double);                                     \
                            RIGHT_CASE(OP, LEFT_CAST, Bool, bool);                                      \
                        default:                                                                        \
                            std::cerr << "UNREACHABLE at " << __FILE__ << ":" << __LINE__ << std::endl; \
                            return left.type;                                                           \
                        }                                                                               \
                    } while (0)

                #define LEFT_CASE(OP, TYPE, CTYPE) \
                    case Type::Kind::TYPE:         \
                        DO_OP_RIGHT(OP, CTYPE)

                #define DO_OP_LEFT(OP)                                                                  \
                    do                                                                                  \
                    {                                                                                   \
                        switch (left.type.kind)                                      \
                        {                                                                               \
                            LEFT_CASE(OP, Char, char);                                                  \
                            LEFT_CASE(OP, U8, unsigned char);                                           \
                            LEFT_CASE(OP, I8, signed char);                                             \
                            LEFT_CASE(OP, U32, unsigned int);                                           \
                            LEFT_CASE(OP, I32, int);                                                    \
                            LEFT_CASE(OP, U64, unsigned long long);                                     \
                            LEFT_CASE(OP, I64, long long);                                              \
                            LEFT_CASE(OP, F32, float);                                                  \
                            LEFT_CASE(OP, F64, double);                                                 \
                            LEFT_CASE(OP, Bool, bool);                                                  \
                        default:                                                                        \
                            std::cerr << "UNREACHABLE at " << __FILE__ << ":" << __LINE__ << std::endl; \
                            return left.type;                                                           \
                        }                                                                               \
                    } while (0)

                switch (op_kind)
                {
                case OperatorKind::Addition:
                    DO_OP_LEFT(+);
                case OperatorKind::Subtraction:
                    DO_OP_LEFT(-);
                default:
                   // UNREACHABLE
                    std::cerr << "UNREACHABLE at " << __FILE__ << ":" << __LINE__ << std::endl;
                    return left.type;
                }

                #undef DO_OP_LEFT
                #undef LEFT_CASE
                #undef DO_OP_RIGHT
                #undef RIGHT_CASE
                #undef DO_CASTED_OP
            }

            virtual BoundNodeKind bound_node_kind() const override
            {
                return BoundNodeKind::BinaryExpression;
            }
        };
    }
}