#pragma once
#include <memory>
#include "binding/bound_expression.hpp"

namespace scc
{
    namespace binding
    {
        struct BoundBinaryExpression : public BoundExpression
        {
            enum class OpKind
            {
                Addition,
                Subtraction,

                COUNT
            };

            std::unique_ptr<BoundExpression> left;
            std::unique_ptr<BoundExpression> right;
            OpKind op_kind;

            BoundBinaryExpression(std::unique_ptr<BoundExpression> left
                                , std::unique_ptr<BoundExpression> right
                                , OpKind op_kind)
            : BoundExpression(derive_type(*left, *right))
            , left(std::move(left))
            , right(std::move(right))
            , op_kind(op_kind)
            {
            }

            static Type derive_type(const BoundExpression& left, const BoundExpression& right)
            {
                // TODOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO
                (void)right;
                return left.type;
            } 

            virtual BoundNodeKind bound_node_kind() const override
            {
                return BoundNodeKind::BinaryExpression;
            }
        };
    }
}