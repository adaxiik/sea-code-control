#include "binding/binder.hpp"

namespace scc
{
    binding::BinderResult<binding::BoundIfStatement> Binder::bind_if_statement(const TreeNode &node)
    {
        // if_statement ==>    if (true) ...
        // ├── parenthesized_expression ==>    (true)
        // │   └── true ==>    true
        // └── expression_statement ==>        a = 5;
        //     └── assignment_expression ==>   a = 5
        //         ├── identifier ==>  a
        //         └── number_literal ==>      5

        SCC_ASSERT_NODE_SYMBOL(Parser::IF_STATEMENT_SYMBOL);
        SCC_ASSERT(node.named_child_count() == 2 || node.named_child_count() == 3);
        // TODOO:
        // if ()        <- this is error, because it has more childs
        // // comment
        // statement
        // else
        // statement
        bool has_else = node.named_child_count() == 3;
        auto condition_expr = bind_expression(node.first_named_child());
        BUBBLE_ERROR(condition_expr);
        
        // we will cast the condition to bool
        auto condition = std::make_unique<binding::BoundCastExpression>(condition_expr.release_value(), Type(Type::PrimitiveType::Bool));

        auto then_statement_result = bind_impl(node.named_child(1));
        BUBBLE_ERROR(then_statement_result);
        
        SCC_BINDER_RESULT_TYPE(bind_if_statement);
        if(!then_statement_result.get_value()->is_statement())
            return binding::BinderResult<ResultType>(binding::BinderError(ErrorKind::ReachedUnreachableError, node));

        std::unique_ptr<binding::BoundStatement> then_statement = std::unique_ptr<binding::BoundStatement>(static_cast<binding::BoundStatement*>(then_statement_result.release_value().release()));

        std::unique_ptr<binding::BoundStatement> else_statement = nullptr;
        if (has_else)
        {
            auto else_statement_result = bind_impl(node.named_child(2));
            BUBBLE_ERROR(else_statement_result);
            if(!else_statement_result.get_value()->is_statement())
                return binding::BinderResult<ResultType>(binding::BinderError(ErrorKind::ReachedUnreachableError, node));

            else_statement = std::unique_ptr<binding::BoundStatement>(static_cast<binding::BoundStatement*>(else_statement_result.release_value().release()));
        }

        return std::make_unique<binding::BoundIfStatement>(std::move(condition), std::move(then_statement), std::move(else_statement));
    }
}