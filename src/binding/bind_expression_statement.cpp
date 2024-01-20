#include "binding/binder.hpp"

namespace scc
{
    binding::BinderResult<binding::BoundExpressionStatement> Binder::bind_expression_statement(const TreeNode &node)
    {
        SCC_ASSERT_NODE_SYMBOL(Parser::EXPRESSION_STATEMENT_SYMBOL);
        SCC_BINDER_RESULT_TYPE(bind_expression_statement);  
        
        if (node.named_child_count() == 0) // eg.: `;`
            return binding::BinderResult<ResultType>::error(binding::BinderError(ErrorKind::EmptyExpressionError, node));

        SCC_ASSERT_NAMED_CHILD_COUNT(node, 1);
        auto expression_statement = std::make_unique<binding::BoundExpressionStatement>();

        auto expression = bind_expression(node.named_child(0));
        BUBBLE_ERROR(expression);

        expression_statement->expression = expression.release_value();
        return  binding::BinderResult<ResultType>::ok(std::move(expression_statement));
    }
}