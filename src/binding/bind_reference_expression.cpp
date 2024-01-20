#include "binding/binder.hpp"

namespace scc
{
    binding::BinderResult<binding::BoundReferenceExpression> Binder::bind_reference_expression(const TreeNode &node)
    {
        SCC_BINDER_RESULT_TYPE(bind_reference_expression);
        SCC_ASSERT_NODE_SYMBOL(Parser::POINTER_EXPRESSION_SYMBOL);
        SCC_ASSERT_NAMED_CHILD_COUNT(node, 1);
        // pointer_expression ==>  &a
        // └── identifier ==>      a

        auto identifier = node.first_named_child();
        if (identifier.symbol() != Parser::IDENTIFIER_SYMBOL)
        {
            // unreachable?
            return binding::BinderResult<ResultType>::error(binding::BinderError(ErrorKind::ReachedUnreachableError, node));
        }

        Type* type_ptr = m_scope_stack.get_from_scopestack(identifier.value());

        if (not type_ptr)
        {
            auto error = binding::BinderResult<ResultType>(binding::BinderError(ErrorKind::UnknownSymbolError, node));
            error.add_diagnostic("Unknown identifier: " + identifier.value());
            return error;
        }

        Type type = *type_ptr;
        type.modifiers.push_back(Type::Pointer{});
        return std::make_unique<binding::BoundReferenceExpression>(identifier.value(), type);
    }
}