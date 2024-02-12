#include "binding/binder.hpp"

namespace scc
{
    binding::BinderResult<binding::BoundForStatement> Binder::bind_for_statement(const TreeNode &node)
    {
        SCC_BINDER_RESULT_TYPE(bind_for_statement);

        // only for(int i = 0; i < 10; i++) and for(;;) are supported

        if (node.named_child_count() != 4 and node.named_child_count() != 1)
            return binding::BinderResult<ResultType>(binding::BinderError(ErrorKind::UnsupportedForStatementError, node));


        // for(;;)
        if (node.named_child_count() == 1)
        {
            auto body_statement_result = bind_impl(node.first_named_child());
            BUBBLE_ERROR(body_statement_result);
            if(!body_statement_result.get_value()->is_statement())
                return binding::BinderResult<ResultType>(binding::BinderError(ErrorKind::ReachedUnreachableError, node));

            std::unique_ptr<binding::BoundBlockStatement> body_statement = std::unique_ptr<binding::BoundBlockStatement>(static_cast<binding::BoundBlockStatement*>(body_statement_result.release_value().release()));

            return std::make_unique<binding::BoundForStatement>(nullptr, nullptr, nullptr, std::move(body_statement));
        }

        // for(int i = 0; i < 10; i++)

        m_scope_stack.push();

        auto initializer_result = bind_impl(node.named_child(0));
        BUBBLE_ERROR(initializer_result);

        auto condition_expr = bind_expression(node.named_child(1));
        BUBBLE_ERROR(condition_expr);

        // we will cast the condition to bool
        auto condition = std::make_unique<binding::BoundCastExpression>(condition_expr.release_value(), Type(Type::PrimitiveType::Bool));

        auto increment_result = bind_expression(node.named_child(2));
        BUBBLE_ERROR(increment_result);

        std::unique_ptr<binding::BoundExpression> increment = std::unique_ptr<binding::BoundExpression>(increment_result.release_value().release());

        auto body_statement_result = bind_impl(node.named_child(3));
        BUBBLE_ERROR(body_statement_result);
        if(not body_statement_result.get_value()->is_statement())
            return binding::BinderResult<ResultType>(binding::BinderError(ErrorKind::ReachedUnreachableError, node));

        m_scope_stack.pop();

        std::unique_ptr<binding::BoundBlockStatement> body_statement = std::unique_ptr<binding::BoundBlockStatement>(static_cast<binding::BoundBlockStatement*>(body_statement_result.release_value().release()));

        return std::make_unique<binding::BoundForStatement>(initializer_result.release_value(), std::move(condition), std::move(increment), std::move(body_statement));
    }
}