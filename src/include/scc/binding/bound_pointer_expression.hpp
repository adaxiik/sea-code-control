#pragma once
#include "binding/bound_expression.hpp"
namespace scc
{
    namespace binding
    {
        struct BoundPointerExpression : public BoundExpression
        {
            Type::Value value;
            BoundPointerExpression(const Type::Value& value)                
                : BoundExpression(Type::from_value(value)) 
                , value(value)
            {
            }

            virtual BoundNodeKind bound_node_kind() const override
            {
                // TODO: Change this
                return BoundNodeKind::LiteralExpression; 
            }
        };
    } 
}