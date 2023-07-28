#pragma once
#include <memory>
#include <vector>
#include "bound_expression.hpp"

namespace scc
{
    namespace binding
    {

        struct BoundParenthesizedExpression : BoundExpression
        {
            std::vector<std::unique_ptr<BoundExpression>> expressions;

            BoundParenthesizedExpression(std::vector<std::unique_ptr<BoundExpression>> expressions)
                : BoundExpression(expressions.back()->type) // must be bigger than 0
                , expressions(std::move(expressions)) {}
            
            BoundParenthesizedExpression(std::unique_ptr<BoundExpression> expression)
                : BoundExpression(expression->type)
            {
                expressions.push_back(std::move(expression));
            }

            bool is_single_expression() const
            {
                return expressions.size() == 1;
            }

            virtual BoundNodeKind bound_node_kind() const override
            {
                return BoundNodeKind::ParenthesizedExpression;
            }
        };
    }
}