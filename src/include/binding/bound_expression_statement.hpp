#pragma once

#include "binding/bound_node.hpp"
#include "binding/bound_expression.hpp"
#include "binding/bound_statement.hpp"

#include <memory>

namespace scc
{
    namespace binding
    {
        struct BoundExpressionStatement : public BoundStatement
        {
            std::unique_ptr<BoundExpression> expression;

            BoundExpressionStatement() = default;

            virtual BoundNodeKind bound_node_kind() const override
            {
                return BoundNodeKind::ExpressionStatement;
            }
        };
    } 
}