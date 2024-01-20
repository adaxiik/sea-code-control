#include "binding/binder.hpp"

namespace scc
{
    binding::BinderResult<binding::BoundParenthesizedExpression> Binder::bind_parenthesized_expression(const TreeNode &node)
    {
        // parenthesized_expression ==>    (1,2,3)
        // └── comma_expression ==>        1,2,3
        //     ├── number_literal ==>      1
        //     └── comma_expression ==>    2,3
        //         ├── number_literal ==>  2
        //         └── number_literal ==>  3

        // or

        // binary_expression ==>   2 * (3 + 3)
        // ├── number_literal ==>  2
        // └── parenthesized_expression ==>        (3 + 3)
        //     └── binary_expression ==>   3 + 3
        //         ├── number_literal ==>  3
        //         └── number_literal ==>  3

        SCC_ASSERT_NODE_SYMBOL(Parser::PARENTHESIZED_EXPRESSION_SYMBOL);
        SCC_ASSERT_NAMED_CHILD_COUNT(node, 1);
        std::vector<std::unique_ptr<binding::BoundExpression>> expressions;
        if (node.first_named_child().symbol() == Parser::COMMA_EXPRESSION_SYMBOL)
        {
            auto current_node = node.first_named_child();

            do
            {
                auto expression = bind_expression(current_node.first_named_child());
                BUBBLE_ERROR(expression);
                expressions.push_back(expression.release_value());
                current_node = current_node.last_named_child();
            
            } while (current_node.symbol() == Parser::COMMA_EXPRESSION_SYMBOL);

            auto last_expression = bind_expression(current_node);
            BUBBLE_ERROR(last_expression);
            expressions.push_back(last_expression.release_value());
            
            return std::make_unique<binding::BoundParenthesizedExpression>(std::move(expressions));
        }
        else
        {
            auto expression = bind_expression(node.first_named_child());
            BUBBLE_ERROR(expression);

            return std::make_unique<binding::BoundParenthesizedExpression>(expression.release_value());
        }

    }
}