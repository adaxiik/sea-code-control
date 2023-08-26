#pragma once
#include <memory>
#include <optional>
#include "binding/bound_expression.hpp"
#include "binding/bound_node.hpp"

namespace scc
{
    namespace binding
    {
        struct BoundIdentifierExpression: public BoundExpression
        {
            std::string identifier;

            BoundIdentifierExpression(const std::string &identifier, Type type)
                : BoundExpression(type), identifier(identifier)
            {
            }

            virtual BoundNodeKind bound_node_kind() const override
            {
                return BoundNodeKind::IdentifierExpression;
            }
        };
    }
}