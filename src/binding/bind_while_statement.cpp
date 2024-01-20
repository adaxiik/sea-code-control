#include "binding/binder.hpp"

namespace scc
{
    binding::BinderResult<binding::BoundWhileStatement> Binder::bind_while_statement(const TreeNode &node)
    {
        // while_statement ==> while (1) ...
        // ├── parenthesized_expression ==>    (1)
        // │   └── 1 ==>    1
        // └── expression_statement ==>        a = 5;
        //     └── assignment_expression ==>   a = 5
        //         ├── identifier ==>  a
        //         └── number_literal ==>      5

        SCC_ASSERT_NODE_SYMBOL(Parser::WHILE_STATEMENT_SYMBOL);
        SCC_ASSERT_NAMED_CHILD_COUNT(node, 2);
        auto condition_expr = bind_expression(node.first_named_child());
        BUBBLE_ERROR(condition_expr);
        
        // we will cast the condition to bool
        auto condition = std::make_unique<binding::BoundCastExpression>(condition_expr.release_value(), Type(Type::PrimitiveType::Bool));

        auto body_statement_result = bind_impl(node.named_child(1));
        BUBBLE_ERROR(body_statement_result);
        
        SCC_BINDER_RESULT_TYPE(bind_while_statement);
        if(!body_statement_result.get_value()->is_statement())
            return binding::BinderResult<ResultType>(binding::BinderError(ErrorKind::ReachedUnreachableError, node));

        std::unique_ptr<binding::BoundStatement> body_statement = std::unique_ptr<binding::BoundStatement>(static_cast<binding::BoundStatement*>(body_statement_result.release_value().release()));

        return std::make_unique<binding::BoundWhileStatement>(std::move(condition), std::move(body_statement));
    }
}