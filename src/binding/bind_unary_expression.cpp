#include "binding/binder.hpp"

namespace scc
{
    binding::BinderResult<binding::BoundUnaryExpression> Binder::bind_unary_expression(const TreeNode &node)
    {
        // unary_expression ==>    !0
        // └── number_literal ==>  0

        SCC_BINDER_RESULT_TYPE(bind_unary_expression);

        SCC_ASSERT_NODE_SYMBOL(Parser::UNARY_EXPRESSION_SYMBOL);
        SCC_ASSERT_NAMED_CHILD_COUNT(node, 1);
        static_assert(static_cast<int>(binding::BoundUnaryExpression::OperatorKind::COUNT) == 2);

        auto bound_expression = bind_expression(node.first_named_child());
        BUBBLE_ERROR(bound_expression);

        using OperatorKind = binding::BoundUnaryExpression::OperatorKind;
        auto op_kind = OperatorKind::COUNT;
        std::string op_kind_string = node.value();
        if (op_kind_string.starts_with('!'))
        {
            op_kind = OperatorKind::LogicalNot;
        }
        else if (op_kind_string.starts_with('~'))
        {
            op_kind = OperatorKind::BinaryNot;
        }
        else
        {
            auto error = binding::BinderResult<ResultType>(binding::BinderError(ErrorKind::UnknownSymbolError, node));
            error.add_diagnostic("Unknown unary operator: " + op_kind_string);
            return error;
        }

        auto type_opt = binding::BoundUnaryExpression::deduce_type(bound_expression.get_value()->type, op_kind);
        if (not type_opt.has_value())
            return binding::BinderResult<ResultType>::error(binding::BinderError(ErrorKind::InvalidOperationError, node));

        return std::make_unique<binding::BoundUnaryExpression>(bound_expression.release_value(), type_opt.value(), op_kind);
    }
}