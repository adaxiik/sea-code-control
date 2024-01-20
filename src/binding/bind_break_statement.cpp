#include "binding/binder.hpp"

namespace scc
{
    binding::BinderResult<binding::BoundBreakStatement> Binder::bind_break_statement(const TreeNode &node)
    {
        SCC_ASSERT_NODE_SYMBOL(Parser::BREAK_STATEMENT_SYMBOL);
        SCC_ASSERT_NAMED_CHILD_COUNT(node, 0);
        return std::make_unique<binding::BoundBreakStatement>();
    }
}