#pragma once

#include "binding/bound_node.hpp"
#include "binding/bound_statement.hpp"

#include <memory>

namespace scc
{
    namespace binding
    {
        struct BoundReturnStatement : public BoundStatement
        {
            std::unique_ptr<BoundExpression> return_expression;

            BoundReturnStatement(std::unique_ptr<BoundExpression> return_expression)
                : return_expression(std::move(return_expression))
            {
            }

            BoundReturnStatement() : return_expression(nullptr)
            {
            }

            bool has_return_expression() const
            {
                return return_expression != nullptr;
            }

            virtual BoundNodeKind bound_node_kind() const override
            {
                return BoundNodeKind::ReturnStatement;
            }
        };
    } 
}