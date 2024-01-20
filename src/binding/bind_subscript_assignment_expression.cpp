#include "binding/binder.hpp"

namespace scc
{
    binding::BinderResult<binding::BoundPointerAssignmentExpression> Binder::bind_subscript_assignment_expression(const TreeNode &node)
    {
        SCC_BINDER_RESULT_TYPE(bind_subscript_assignment_expression);
        SCC_ASSERT_NODE_SYMBOL(Parser::ASSIGNMENT_EXPRESSION_SYMBOL);
        SCC_ASSERT_CHILD_COUNT(node, 3);

        // expression_statement ==>    data[0] = ...
        // └── assignment_expression ==>       data[0] = ...
        //     ├── subscript_expression ==>    data[0]
        //     │   ├── identifier ==>  data
        //     │   └── number_literal ==>      0
        //     └── number_literal ==>  4

        const std::string op_kind = node.child(1).value();

        SCC_ASSERT_EQ(node.first_named_child().symbol(), Parser::SUBSCRIPT_EXPRESSION_SYMBOL);

        auto subscript_expression = bind_subscript_expression(node.first_named_child());
        BUBBLE_ERROR(subscript_expression);

        // we want to skip dereference, so we just take the underlying expression
        auto left_expression = std::move(subscript_expression.get_value()->expression);

        auto bound_right_expression = bind_expression(node.last_named_child());
        BUBBLE_ERROR(bound_right_expression);

        auto right_expression = bound_right_expression.release_value();

        Type target_type = left_expression->type;
        target_type.modifiers.pop_back();

        if (op_kind == "=")
        {
            auto casted_expression = std::make_unique<binding::BoundCastExpression>(std::move(right_expression), target_type);
            return std::make_unique<binding::BoundPointerAssignmentExpression>(std::move(left_expression), std::move(casted_expression), target_type);
        }

        auto operator_kind_opt = Binder::operation_kind_from_string(op_kind);
        if (not operator_kind_opt.has_value())
            return binding::BinderResult<ResultType>::error(binding::BinderError(ErrorKind::InvalidOperationError, node));

        auto operator_kind = operator_kind_opt.value();

        auto binary_expression = std::make_unique<binding::BoundBinaryExpression>(std::make_unique<binding::BoundDereferenceExpression>(std::move(left_expression), target_type)
                                                                                , std::move(right_expression)
                                                                                , target_type
                                                                                , operator_kind);

        auto casted_binary_expression = std::make_unique<binding::BoundCastExpression>(std::move(binary_expression), target_type);

        auto subscript_expression_again = bind_subscript_expression(node.first_named_child());
        BUBBLE_ERROR(subscript_expression_again);

        return std::make_unique<binding::BoundPointerAssignmentExpression>(std::move(subscript_expression_again.get_value()->expression), std::move(casted_binary_expression), target_type);
    }
}