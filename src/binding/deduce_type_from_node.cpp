#include "binding/binder.hpp"

namespace scc
{
    static std::optional<Type> struct_from_string(Binder& binder, const TreeNode &node)
    {
        SCC_ASSERT_NODE_SYMBOL(Parser::STRUCT_SPECIFIER_SYMBOL);
        // SCC_ASSERT_NAMED_CHILD_COUNT(node, 1);

        // we only support anonymous structs for now
        if (node.named_child_count() != 1)
            return std::nullopt;

        // struct_specifier ==>    struct { i...
        // └── field_declaration_list ==>  { int a; A...
        //     ├── field_declaration ==>   int a;
        //     │   ├── primitive_type ==>  int
        //     │   └── field_identifier ==>        a
        //     └── field_declaration ==>   Amen x;
        //         ├── type_identifier ==> Amen
        //         └── field_identifier ==>        x

        std::string type_name = node.last_named_child().value();
        std::map<std::string, Type> fields;

        for (uint32_t i = 0; i < node.first_named_child().named_child_count(); i++)
        {
            auto field_declaration = node.first_named_child().named_child(i);
            SCC_ASSERT(field_declaration.symbol() == Parser::FIELD_DECLARATION_SYMBOL);
            SCC_ASSERT_NAMED_CHILD_COUNT(field_declaration, 2);

            auto field_type = field_declaration.first_named_child();
            auto field_name = field_declaration.last_named_child();

            auto type = binder.deduce_type_from_node(field_type);
            if (not type.has_value())
                return std::nullopt;

            fields.insert_or_assign(field_name.value(), type.value());
        }

        return Type::StructType(type_name, fields);
    }

    std::optional<Type> Binder::deduce_type_from_node(const TreeNode &node)
    {
        // SCC_ASSERT_NODE_SYMBOL(Parser::TYPE_DESCRIPTOR_SYMBOL );

        SCC_ASSERT(
            node.symbol() == Parser::TYPE_DESCRIPTOR_SYMBOL
         or node.symbol() == Parser::STRUCT_SPECIFIER_SYMBOL
         or node.symbol() == Parser::PRIMITIVE_TYPE_SYMBOL
         or node.symbol() == Parser::SIZED_TYPE_SPECIFIER_SYMBOL
         or node.symbol() == Parser::TYPE_IDENTIFIER_SYMBOL);


        if (
            node.symbol() == Parser::PRIMITIVE_TYPE_SYMBOL
            or node.symbol() == Parser::SIZED_TYPE_SPECIFIER_SYMBOL
            or node.symbol() == Parser::TYPE_IDENTIFIER_SYMBOL
        )
        {
            std::string type_name = node.value();
            if (m_typedefs.contains(type_name))
                return m_typedefs.at(type_name);

            return Type::from_string(node.value());
        }

        if (
            node.named_child_count() == 1
            and (
                node.first_named_child().symbol() == Parser::PRIMITIVE_TYPE_SYMBOL
                or node.first_named_child().symbol() == Parser::SIZED_TYPE_SPECIFIER_SYMBOL
            )
        )
        {
            std::string type_name = node.first_named_child().value();
            if (m_typedefs.contains(type_name))
                return m_typedefs.at(type_name);

            return Type::from_string(node.first_named_child().value());
        }



        if (node.symbol() == Parser::STRUCT_SPECIFIER_SYMBOL)
            return struct_from_string(*this, node);


        return std::nullopt;
    }
}