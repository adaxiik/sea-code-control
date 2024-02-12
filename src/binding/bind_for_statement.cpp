#include "binding/binder.hpp"

namespace scc
{
    binding::BinderResult<binding::BoundForStatement> Binder::bind_for_statement(const TreeNode &node)
    {
        // for_statement ==>   for (int i...
        // ├── declaration ==> int i = 0;
        // │   ├── primitive_type ==>  int
        // │   └── init_declarator ==> i = 0
        // │       ├── identifier ==>  i
        // │       └── number_literal ==>      0
        // ├── binary_expression ==>   i < 10
        // │   ├── identifier ==>      i
        // │   └── number_literal ==>  10
        // ├── update_expression ==>   i++
        // │   └── identifier ==>      i
        // └── compound_statement ==>  { }


        SCC_BINDER_RESULT_TYPE(bind_for_statement);
        SCC_ASSERT_NODE_SYMBOL(Parser::FOR_STATEMENT_SYMBOL);

        std::optional<TreeNode> initializer_opt = node.child_by_field(Parser::INITIALIZER_FIELD);
        std::optional<TreeNode> condition_opt = node.child_by_field(Parser::CONDITION_FIELD);
        std::optional<TreeNode> update_opt = node.child_by_field(Parser::UPDATE_FIELD);
        std::optional<TreeNode> body_opt = node.named_child_with_symbol_BFS(Parser::COMPOUND_STATEMENT_SYMBOL, TreeNode::SearchDepth::one);
        m_scope_stack.push();

        std::unique_ptr<binding::BoundNode> initializer;
        if (initializer_opt.has_value())
        {
            auto initializer_result = bind_impl(initializer_opt.value());
            BUBBLE_ERROR(initializer_result);
            initializer = initializer_result.release_value();
        }

        std::unique_ptr<binding::BoundExpression> condition;
        if (condition_opt.has_value())
        {
            auto condition_expr = bind_expression(condition_opt.value());
            BUBBLE_ERROR(condition_expr);

            // we will cast the condition to bool
            condition = std::make_unique<binding::BoundCastExpression>(condition_expr.release_value(), Type(Type::PrimitiveType::Bool));
        }

        std::unique_ptr<binding::BoundExpression> update;
        if (update_opt.has_value())
        {
            auto increment_result = bind_expression(update_opt.value());
            BUBBLE_ERROR(increment_result);
            update = increment_result.release_value();
        }

        std::unique_ptr<binding::BoundBlockStatement> body_statement;
        if (body_opt.has_value())
        {
            auto body_statement_result = bind_impl(body_opt.value());
            BUBBLE_ERROR(body_statement_result);
            if (not body_statement_result.get_value()->is_statement())
                return binding::BinderResult<ResultType>(binding::BinderError(ErrorKind::ReachedUnreachableError, node));

            body_statement = std::unique_ptr<binding::BoundBlockStatement>(static_cast<binding::BoundBlockStatement *>(body_statement_result.release_value().release()));
        }

        m_scope_stack.pop();

        return std::make_unique<binding::BoundForStatement>(std::move(initializer), std::move(condition), std::move(update), std::move(body_statement));
    }
}