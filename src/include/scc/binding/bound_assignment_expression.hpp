#pragma once
#include "binding/bound_expression.hpp"
namespace scc
{
    namespace binding
    {
        struct BoundAssignmentExpression : public BoundExpression
        {
            std::string identifier;
            std::unique_ptr<BoundExpression> expression;
            BoundAssignmentExpression(const std::string& identifier, Type identifier_type, std::unique_ptr<BoundExpression> expression)
                : BoundExpression(identifier_type), identifier(identifier), expression(std::move(expression))
            {
            }

            virtual BoundNodeKind bound_node_kind() const override
            {
                return BoundNodeKind::AssignmentExpression;
            }
        };
    } 
}