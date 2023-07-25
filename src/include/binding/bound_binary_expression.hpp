#pragma once
#include <memory>
#include "binding/bound_expression.hpp"
#include "binding/bound_binary_operator.hpp"
namespace scc
{
    namespace binding
    {
        struct BoundBinaryExpression : public BoundExpression
        {
            std::unique_ptr<BoundExpression> left;
            std::unique_ptr<BoundExpression> right;
            // std::unique_ptr<BoundBinaryOperator> op;

            virtual BoundNodeKind bound_node_kind() const override
            {
                return BoundNodeKind::BinaryExpression;
            }
        };
    }
}