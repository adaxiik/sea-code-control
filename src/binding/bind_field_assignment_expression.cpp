#include "binding/binder.hpp"

namespace scc
{
    binding::BinderResult<binding::BoundPointerAssignmentExpression> Binder::bind_field_assignment_expression(const TreeNode &node)
    {
        // assignment_expression ==>       a.a = 5
        // ├── field_expression ==>        a.a
        // │   ├── identifier ==>  a
        // │   └── field_identifier ==>    a
        // └── number_literal ==>  5

        SCC_BINDER_RESULT_TYPE(bind_field_assignment_expression);
        SCC_ASSERT_NODE_SYMBOL(Parser::ASSIGNMENT_EXPRESSION_SYMBOL);
        SCC_ASSERT_CHILD_COUNT(node, 3);

        const std::string op_kind = node.child(1).value();

        SCC_ASSERT_EQ(node.first_named_child().symbol(), Parser::FIELD_EXPRESSION_SYMBOL);

        auto left_expression = bind_field_expression(node.first_named_child());
        BUBBLE_ERROR(left_expression);

        auto right_expression = bind_expression(node.last_named_child());
        BUBBLE_ERROR(right_expression);

        Type target_type = left_expression.get_value()->expression->type;
        target_type.modifiers.pop_back();

        if (op_kind == "=")
        {
            auto casted_expression = std::make_unique<binding::BoundCastExpression>(right_expression.release_value(), target_type);
            return std::make_unique<binding::BoundPointerAssignmentExpression>(std::move(left_expression.get_value()->expression), std::move(casted_expression), target_type);
        }

        auto operator_kind_opt = Binder::operation_kind_from_string(op_kind);
        if (not operator_kind_opt.has_value())
            return binding::BinderResult<ResultType>::error(binding::BinderError(ErrorKind::InvalidOperationError, node));

        auto operator_kind = operator_kind_opt.value();

        auto binary_expression = std::make_unique<binding::BoundBinaryExpression>(
            std::move(left_expression.get_value()->expression),
            std::move(right_expression.release_value()),
            target_type,
            operator_kind
        );

        auto casted_binary_expression = std::make_unique<binding::BoundCastExpression>(
            std::move(binary_expression),
            target_type
        );

        return std::make_unique<binding::BoundPointerAssignmentExpression>(
            std::move(left_expression.get_value()->expression),
            std::move(casted_binary_expression),
            target_type
        );
    }

}