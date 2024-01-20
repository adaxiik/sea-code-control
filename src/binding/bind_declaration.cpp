#include "binding/binder.hpp"

namespace scc
{
    binding::BinderResult<binding::BoundNode> Binder::bind_declaration(const TreeNode &node)
    {
        SCC_ASSERT_NODE_SYMBOL(Parser::DECLARATION_SYMBOL);

        if (node.has_named_child_with_symbol(Parser::FUNCTION_DECLARATOR_SYMBOL))
            return bind_function(node);
        
        return bind_variable_declaration(node);
    }
}