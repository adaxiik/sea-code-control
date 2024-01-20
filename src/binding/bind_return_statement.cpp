#include "binding/binder.hpp"

namespace scc
{
    binding::BinderResult<binding::BoundReturnStatement> Binder::bind_return_statement(const TreeNode &node)
    {
        SCC_ASSERT_NODE_SYMBOL(Parser::RETURN_STATEMENT_SYMBOL);
        SCC_BINDER_RESULT_TYPE(bind_return_statement);
        if (!m_current_function.has_value())
            return binding::BinderResult<ResultType>(binding::BinderError(ErrorKind::ReturnStatementOutsideFunctionError, node));

        if (node.named_child_count() == 1)
        {
            auto expression = bind_expression(node.first_named_child());
            BUBBLE_ERROR(expression);

            return std::make_unique<binding::BoundReturnStatement>(std::make_unique<binding::BoundCastExpression>(expression.release_value()
                                                                , m_current_function.value().get().return_type));
        }

        if (m_current_function.value().get().return_type != Type(Type::PrimitiveType::Void))
        {
            auto error = binding::BinderResult<ResultType>(binding::BinderError(ErrorKind::ReturnStatementMissingExpressionError, node));
            return error;
        }

        return std::make_unique<binding::BoundReturnStatement>();
    }
}