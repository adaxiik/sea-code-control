#include "binding/binder.hpp"

namespace scc
{
    binding::BinderResult<binding::BoundDereferenceExpression> Binder::bind_subscript_expression(const TreeNode &node)
    {
        SCC_BINDER_RESULT_TYPE(bind_subscript_expression);
        SCC_ASSERT_NODE_SYMBOL(Parser::SUBSCRIPT_EXPRESSION_SYMBOL);
        SCC_ASSERT_NAMED_CHILD_COUNT(node, 2);

        // subscript_expression ==>  a[5]
        // ├── identifier ==>      a
        // └── number_literal ==>  5

        // subscript_expression ==>  4[a]
        // ├── number_literal ==>  4
        // └── identifier ==>      a

        

        auto array_node = node.first_named_child();
        auto index_node = node.last_named_child();

        auto array_expression = bind_expression(array_node);
        BUBBLE_ERROR(array_expression);
        auto index_expression = bind_expression(index_node);
        BUBBLE_ERROR(index_expression);

        if (array_expression.get_value()->type.pointer_depth() < 1 and index_expression.get_value()->type.pointer_depth() < 1)
        {
            auto error = binding::BinderResult<ResultType>(binding::BinderError(ErrorKind::InvalidOperationError, node));
            error.add_diagnostic("Cannot subscript non-pointer type");
            return error;
        }

        if (array_expression.get_value()->type.pointer_depth() > 0 and index_expression.get_value()->type.pointer_depth() > 0)
        {
            auto error = binding::BinderResult<ResultType>(binding::BinderError(ErrorKind::InvalidOperationError, node));
            error.add_diagnostic("Cannot subscript pointer type with pointer type");
            return error;
        }


        // a[X] ==> *(a + X)

        auto deduced_type_opt = binding::BoundBinaryExpression::deduce_type(
            array_expression.get_value()->type,
            index_expression.get_value()->type,
            binding::BoundBinaryExpression::OperatorKind::Addition
        );

        if (not deduced_type_opt.has_value())
            return binding::BinderResult<ResultType>(binding::BinderError(ErrorKind::InvalidOperationError, node));

        Type type = array_expression.get_value()->type.pointer_depth() > 0 ? array_expression.get_value()->type : index_expression.get_value()->type;
        type.modifiers.pop_back();

        auto addition_expression = std::make_unique<binding::BoundBinaryExpression>(
            array_expression.release_value(),
            index_expression.release_value(),
            deduced_type_opt.value(),
            binding::BoundBinaryExpression::OperatorKind::Addition
        );

        
        return std::make_unique<binding::BoundDereferenceExpression>(std::move(addition_expression), type);
    }
}