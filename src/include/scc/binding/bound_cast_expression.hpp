#pragma once

#include "binding/bound_expression.hpp"
#include "type.hpp"

namespace scc
{
    namespace binding
    {
        struct BoundCastExpression : BoundExpression
        {
            BoundCastExpression(std::unique_ptr<BoundExpression> expression, Type type)
                : BoundExpression(type), expression(std::move(expression)) {}

            std::unique_ptr<BoundExpression> expression;

            virtual BoundNodeKind bound_node_kind() const override 
            { 
                return BoundNodeKind::CastExpression;
            }
        };
    }
}