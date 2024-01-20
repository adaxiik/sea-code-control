#include "binding/binder.hpp"

namespace scc
{
    std::optional<Type> Binder::deduce_type_from_type_descriptor(const TreeNode &node)
    {
        SCC_ASSERT_NODE_SYMBOL(Parser::TYPE_DESCRIPTOR_SYMBOL);
        if (
            node.named_child_count() == 1 
            and (
                node.first_named_child().symbol() == Parser::PRIMITIVE_TYPE_SYMBOL 
                or node.first_named_child().symbol() == Parser::SIZED_TYPE_SPECIFIER_SYMBOL
            )
        )
        {
            return Type::from_string(node.first_named_child().value());
        }

        return std::nullopt;
    }
}