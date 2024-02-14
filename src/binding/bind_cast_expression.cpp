#include "binding/binder.hpp"

namespace scc
{
    binding::BinderResult<binding::BoundCastExpression> Binder::bind_cast_expression(const TreeNode &node)
    {
        // cast_expression ==>     (float)1
        // ├── type_descriptor ==> float
        // │   └── primitive_type ==>      float
        // └── number_literal ==>  1

        SCC_ASSERT_NODE_SYMBOL(Parser::CAST_EXPRESSION_SYMBOL);
        SCC_ASSERT_NAMED_CHILD_COUNT(node, 2);

        auto bound_expression = bind_expression(node.named_child(1));
        BUBBLE_ERROR(bound_expression);

        auto type_descriptor = node.first_named_child();
        SCC_ASSERT_EQ(type_descriptor.symbol(), Parser::TYPE_DESCRIPTOR_SYMBOL);
        // SCC_ASSERT_CHILD_COUNT(type_descriptor, 1); // might be a pointer

        std::string type_name = type_descriptor.first_named_child().value();
        uint32_t pointer_depth = 0;
        if (type_descriptor.named_child_count() > 1)
        {
            auto current_node = type_descriptor.named_child(1);
            while (true)
            {
                pointer_depth++;
                if (current_node.named_child_count() < 1)
                    break;

                current_node = current_node.named_child(0);
            }
        }

        auto type = Type::from_string(type_name);
        if (not type.has_value())
        {
            // might be typedef
            if (m_typedefs.find(type_name) != m_typedefs.end())
            {
                type = m_typedefs.at(type_name);
            }
        }

        if (not type.has_value())
        {
            // std::cerr << "Unknown type: " << type_name << std::endl;
            // return nullptr;
            SCC_BINDER_RESULT_TYPE(bind_cast_expression);
            auto error = binding::BinderResult<ResultType>(binding::BinderError(ErrorKind::UnknownSymbolError, type_descriptor));
            error.add_diagnostic("Unknown type: " + type_name);
            return error;
        }

        // type.value().pointer_depth = pointer_depth;
        // TODOOOOOOOOOOOOOOOOOOOOOOOO: Arrays???
       type.value().modifiers = std::vector<Type::Modifier>(pointer_depth, Type::Pointer{});

        return std::make_unique<binding::BoundCastExpression>(bound_expression.release_value(), type.value());
    }
}