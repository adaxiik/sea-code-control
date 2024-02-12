#include "binding/binder.hpp"

namespace scc
{
    binding::BinderResult<binding::BoundLiteralExpression> Binder::bind_sizeof_expression(const TreeNode &node)
    {
        SCC_BINDER_RESULT_TYPE(bind_sizeof_expression);
        SCC_ASSERT_NODE_SYMBOL(Parser::SIZEOF_EXPRESSION_SYMBOL);
        SCC_ASSERT_NAMED_CHILD_COUNT(node, 1);

        if (node.first_named_child().symbol() == Parser::PARENTHESIZED_EXPRESSION_SYMBOL
            and node.first_named_child().first_named_child().symbol() == Parser::IDENTIFIER_SYMBOL)
        {
            // might by struct type
            auto type = deduce_type_from_node(node.first_named_child().first_named_child());
            if (type.has_value())
            {
                size_t size = type.value().size_bytes();
                return std::make_unique<binding::BoundLiteralExpression>(static_cast<Type::Primitive::U64>(size));
            }
            // if its not a type, it might be a variable, so we just continue
        }

        if (node.first_named_child().symbol() == Parser::PARENTHESIZED_EXPRESSION_SYMBOL)
        {
            auto binded = bind_parenthesized_expression(node.first_named_child());
            BUBBLE_ERROR(binded);
            size_t size = binded.get_value()->type.size_bytes();
            return std::make_unique<binding::BoundLiteralExpression>(static_cast<Type::Primitive::U64>(size));
        }
        else if (node.first_named_child().symbol() == Parser::TYPE_DESCRIPTOR_SYMBOL)
        {
            auto deduced_type = deduce_type_from_node(node.first_named_child());
            if (not deduced_type.has_value())
            {
                auto error = binding::BinderResult<ResultType>(binding::BinderError(ErrorKind::FailedToDeduceTypeFromTypeDescriptorError, node));
                error.add_diagnostic("Failed to deduce type from type descriptor \"" + node.first_named_child().value() + "\"");
                return error;
            }

            size_t size = deduced_type.value().size_bytes();
            return std::make_unique<binding::BoundLiteralExpression>(static_cast<Type::Primitive::U64>(size));
        }

        auto error = binding::BinderResult<ResultType>(binding::BinderError(ErrorKind::ReachedUnreachableError, node));
        error.add_diagnostic("Unknown sizeof expression \"" + node.first_named_child().value() + "\"");
        return error;
    }
}