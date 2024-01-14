#pragma once
#include "binding/bound_expression.hpp"
namespace scc
{
    namespace binding
    {
        struct BoundDereferenceExpression : public BoundExpression
        {
            std::unique_ptr<BoundExpression> expression;
            BoundDereferenceExpression(std::unique_ptr<BoundExpression> expression, Type type)
                : BoundExpression(type), expression(std::move(expression)) {}

            virtual BoundNodeKind bound_node_kind() const override
            {
                return BoundNodeKind::DereferenceExpression; 
            }
        };
    } 
}