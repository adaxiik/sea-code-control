#include "binding/binder.hpp"

namespace scc
{
    binding::BinderResult<binding::BoundBlockStatement> Binder::bind_block_statement(const TreeNode &node)
    {
        // SCC_ASSERT_NODE_SYMBOL(Parser::TRANSLATION_UNIT_SYMBOL);
        SCC_BINDER_RESULT_TYPE(bind_block_statement);
        SCC_ASSERT(node.symbol() == Parser::TRANSLATION_UNIT_SYMBOL || node.symbol() == Parser::COMPOUND_STATEMENT_SYMBOL);

        auto block_statement = std::make_unique<binding::BoundBlockStatement>();
        if (
            node.symbol() == Parser::TRANSLATION_UNIT_SYMBOL
            and node.named_child_count() == 1
        )
        {
            if (node.first_named_child().symbol() == Parser::PREPROC_INCLUDE_SYMBOL)
            {
                if (not prepoc_include(block_statement->statements, node.first_named_child()))
                    return binding::BinderResult<ResultType>::error(binding::BinderError(ErrorKind::FailedToIncludeError, node));

                return binding::BinderResult<ResultType>::ok(std::move(block_statement));
            }
            else if (node.first_named_child().symbol() == Parser::PREPROC_DEF_SYMBOL)
            {
                if (not prepoc_define(node.first_named_child()))
                    return binding::BinderResult<ResultType>::error(binding::BinderError(ErrorKind::DefineMacroError, node));

                return binding::BinderResult<ResultType>::ok(std::move(block_statement));
            }

            auto result = bind_impl(node.first_named_child());
            BUBBLE_ERROR(result);
            if (not result.get_value()->is_statement())
            {
                SCC_UNREACHABLE();
            }

            auto statement = static_cast<binding::BoundStatement*>(result.release_value().release());
            block_statement->statements.push_back(std::unique_ptr<binding::BoundStatement>(statement));
            return binding::BinderResult<ResultType>::ok(std::move(block_statement));             
        }

        if (node.has_parent())
            m_scope_stack.push();

        for (uint32_t i = 0; i < node.named_child_count(); i++)
        {
            auto child = node.named_child(i);
            if (child.symbol() == Parser::COMMENT_SYMBOL)
                continue;

            if (child.symbol() == Parser::PREPROC_INCLUDE_SYMBOL)
            {
                if (not prepoc_include(block_statement->statements, child))
                    return binding::BinderResult<ResultType>::error(binding::BinderError(ErrorKind::FailedToIncludeError, node));
                continue;
            }
            else if (child.symbol() == Parser::PREPROC_DEF_SYMBOL)
            {
                if (not prepoc_define(child))
                    return binding::BinderResult<ResultType>::error(binding::BinderError(ErrorKind::DefineMacroError, node));
                continue;
            }


            auto binded = bind_impl(child);
            BUBBLE_ERROR(binded);

            if(not binded.get_value()->is_statement())
            {
                // TODOOO: UNREACHABLE???
                SCC_UNREACHABLE();                
            }
            auto bound_statement_ptr = static_cast<binding::BoundStatement*>(binded.release_value().release());
            block_statement->statements.push_back(std::unique_ptr<binding::BoundStatement>(bound_statement_ptr));
        }

        if (node.has_parent())
            m_scope_stack.pop();

        return binding::BinderResult<ResultType>::ok(std::move(block_statement));
    }
}