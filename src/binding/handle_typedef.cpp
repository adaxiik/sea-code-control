#include "binding/binder.hpp"

namespace scc
{
    bool Binder::handle_typedef(const TreeNode &node)
    {
        SCC_ASSERT_NODE_SYMBOL(Parser::TYPE_DEFINITION_SYMBOL);
        SCC_ASSERT_NAMED_CHILD_COUNT(node, 2);

        // type_definition ==> typedef st...
        // ├── struct_specifier ==>    struct {}
        // │   └── field_declaration_list ==>  {}
        // └── type_identifier ==>     X

        std::string type_name = node.last_named_child().value();
        if (m_typedefs.contains(type_name))
            return false;

        auto type = deduce_type_from_node(node.first_named_child());
        if (not type.has_value())
            return false;

        m_typedefs.insert_or_assign(type_name, type.value());

        return true;
    }
}
