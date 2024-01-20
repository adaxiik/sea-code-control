#include "binding/binder.hpp"

namespace scc
{
    binding::BinderResult<binding::BoundDoStatement> Binder::bind_do_statement(const TreeNode &node)
    {
        // do_statement ==>    do {int a ...
        // ├── compound_statement ==>  {int a = 5...
        // │   └── declaration ==>     int a = 5;
        // │       ├── primitive_type ==>      int
        // │       └── init_declarator ==>     a = 5
        // │           ├── identifier ==>      a
        // │           └── number_literal ==>  5
        // └── parenthesized_expression ==>    (0)
        //     └── number_literal ==>  0

        SCC_ASSERT_NODE_SYMBOL(Parser::DO_STATEMENT_SYMBOL);
        SCC_ASSERT_NAMED_CHILD_COUNT(node, 2);
        auto body_statement_result = bind_impl(node.first_named_child());
        BUBBLE_ERROR(body_statement_result);

        SCC_BINDER_RESULT_TYPE(bind_do_statement);

        if(!body_statement_result.get_value()->is_statement())
            return binding::BinderResult<ResultType>(binding::BinderError(ErrorKind::ReachedUnreachableError, node));

        std::unique_ptr<binding::BoundStatement> body_statement = std::unique_ptr<binding::BoundStatement>(static_cast<binding::BoundStatement*>(body_statement_result.release_value().release()));

        auto condition_expr = bind_expression(node.named_child(1));
        BUBBLE_ERROR(condition_expr);

        // we will cast the condition to bool
        auto condition = std::make_unique<binding::BoundCastExpression>(condition_expr.release_value(), Type(Type::PrimitiveType::Bool));

        return std::make_unique<binding::BoundDoStatement>(std::move(condition), std::move(body_statement));
    }
}