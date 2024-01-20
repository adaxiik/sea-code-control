#include "binding/binder.hpp"

namespace scc
{
    binding::BinderResult<binding::BoundPointerAssignmentExpression> Binder::bind_pointer_assignment_expression(const TreeNode &node)
    {
        SCC_BINDER_RESULT_TYPE(bind_pointer_assignment_expression);
        SCC_ASSERT_NODE_SYMBOL(Parser::ASSIGNMENT_EXPRESSION_SYMBOL);
        SCC_ASSERT_CHILD_COUNT(node, 3);
        // assignment_expression ==>       *(&a) = 5
        // ├── pointer_expression ==>      *(&a)
        // │   └── parenthesized_expression ==>    (&a)
        // │       └── pointer_expression ==>      &a
        // │           └── identifier ==>  a
        // └── number_literal ==>  5

        // we want to skip dereference
        auto bound_address_expression = bind_expression(node.child(0).first_named_child());
        BUBBLE_ERROR(bound_address_expression);

        if (not bound_address_expression.get_value()->type.is_pointer())
        {
            auto error = binding::BinderResult<ResultType>(binding::BinderError(ErrorKind::InvalidOperationError, node));
            error.add_diagnostic("Cannot dereference non-pointer type");
            return error;
        }

        Type target_type = bound_address_expression.get_value()->type;
        target_type.modifiers.pop_back();
        
        auto bound_expression = bind_expression(node.last_child());
        BUBBLE_ERROR(bound_expression);

        const std::string op_kind = node.child(1).value();

        if (op_kind == "=")
        {
            auto casted_expression = std::make_unique<binding::BoundCastExpression>(bound_expression.release_value(), target_type);
            return std::make_unique<binding::BoundPointerAssignmentExpression>(bound_address_expression.release_value(), std::move(casted_expression), target_type);
        }


        auto operator_kind_opt = Binder::operation_kind_from_string(op_kind);
        if (not operator_kind_opt.has_value())
            return binding::BinderResult<ResultType>::error(binding::BinderError(ErrorKind::InvalidOperationError, node));

        auto operator_kind = operator_kind_opt.value();
        
        auto binary_expression = std::make_unique<binding::BoundBinaryExpression>(std::make_unique<binding::BoundDereferenceExpression>(bound_address_expression.release_value(), target_type)
                                                                                , bound_expression.release_value()
                                                                                , target_type
                                                                                , operator_kind);

        auto casted_binary_expression = std::make_unique<binding::BoundCastExpression>(std::move(binary_expression), target_type);

        auto addr_bound_again = bind_expression(node.child(0).first_named_child());
        BUBBLE_ERROR(addr_bound_again);

        return std::make_unique<binding::BoundPointerAssignmentExpression>(addr_bound_again.release_value(), std::move(casted_binary_expression), target_type);
    }
}