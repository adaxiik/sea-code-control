#pragma once
#include "binding/bound_expression.hpp"
namespace scc
{
    namespace binding
    {
        struct BoundLiteralExpression : public BoundExpression
        {
            Type::Value value;
            BoundLiteralExpression(const Type::Value& value)                
                : BoundExpression(Type::from_value(value)) 
                , value(value)
            {
            }

            virtual BoundNodeKind bound_node_kind() const override
            {
                return BoundNodeKind::LiteralExpression;
            }
        };
    } 
}