#pragma once
#include "binding/bound_expression.hpp"
namespace scc
{
    namespace binding
    {
        struct BoundStringExpression : public BoundExpression
        {
            std::string value;
            BoundStringExpression(const std::string& value)
                : BoundExpression(Type(
                    Type::PrimitiveType::Char,
                    std::vector<Type::Modifier>{Type::Array{value.size()}}
                ))
                , value(value)
            {
            }

            virtual BoundNodeKind bound_node_kind() const override
            {
                return BoundNodeKind::StringExpression;
            }
        };
    }
}