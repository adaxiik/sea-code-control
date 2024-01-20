#include "binding/binder.hpp"

namespace scc
{
    binding::BinderResult<binding::BoundDereferenceExpression> Binder::bind_dereference_expression(const TreeNode &node)
    {
        SCC_BINDER_RESULT_TYPE(bind_reference_expression);
        SCC_ASSERT_NODE_SYMBOL(Parser::POINTER_EXPRESSION_SYMBOL);
        SCC_ASSERT_NAMED_CHILD_COUNT(node, 1);
        
        // pointer_expression ==>  *a
        // └── identifier ==>      a    // expression 

        auto bound_expression = bind_expression(node.first_named_child());
        BUBBLE_ERROR(bound_expression);

        auto expression = bound_expression.release_value();
        if (expression->type.modifiers.size() == 0)
        {
            auto error = binding::BinderResult<ResultType>(binding::BinderError(ErrorKind::InvalidOperationError, node));
            error.add_diagnostic("Cannot dereference non-pointer type");
            return error;
        }

        auto type = expression->type;
        type.modifiers.pop_back();
        return std::make_unique<binding::BoundDereferenceExpression>(std::move(expression), type);
    }

}