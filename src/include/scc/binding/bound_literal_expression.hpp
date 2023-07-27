#pragma once
#include <any>
#include "binding/bound_expression.hpp"
namespace scc
{
    namespace binding
    {
        struct BoundLiteralExpression : public BoundExpression
        {
            std::any value;
            BoundLiteralExpression(const std::any& value, Type type)                
                : BoundExpression(type), value(value)
            {
            }

            virtual BoundNodeKind bound_node_kind() const override
            {
                return BoundNodeKind::LiteralExpression;
            }
        };
    } 
}