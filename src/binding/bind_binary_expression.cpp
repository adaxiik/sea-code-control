#include "binding/binder.hpp"

namespace scc
{
    binding::BinderResult<binding::BoundBinaryExpression> Binder::bind_binary_expression(const TreeNode &node)
    {
        SCC_ASSERT_NODE_SYMBOL(Parser::BINARY_EXPRESSION_SYMBOL);
        SCC_ASSERT_CHILD_COUNT(node, 3);

        auto bound_left_result = bind_expression(node.child(0));
        BUBBLE_ERROR(bound_left_result);
        auto bound_right_result = bind_expression(node.child(2));
        BUBBLE_ERROR(bound_right_result);

        auto bound_left = bound_left_result.release_value();
        auto bound_right = bound_right_result.release_value();

        std::string op = node.child(1).value();

        using OpKind = binding::BoundBinaryExpression::OperatorKind;
        SCC_BINDER_RESULT_TYPE(bind_binary_expression);

        #define BIND_BINARY_OPERATOR(OP_STR, OP_KIND) \
            if (op == (OP_STR)){ \
                auto deduced_type = binding::BoundBinaryExpression::deduce_type(bound_left->type, bound_right->type, OpKind::OP_KIND); \
                if(not deduced_type.has_value()){ \
                    return binding::BinderResult<ResultType>::error(binding::BinderError(ErrorKind::InvalidOperationError, node)); \
                } \
                return std::make_unique<binding::BoundBinaryExpression>(std::move(bound_left)   \
                                                                      , std::move(bound_right)  \
                                                                      , deduced_type.value()       \
                                                                      , OpKind::OP_KIND); \
            }

        static_assert(static_cast<int>(OpKind::COUNT) == 18, "Update this code");


        BIND_BINARY_OPERATOR("+", Addition);
        BIND_BINARY_OPERATOR("-", Subtraction);
        BIND_BINARY_OPERATOR("*", Multiplication);
        BIND_BINARY_OPERATOR("/", Division);
        BIND_BINARY_OPERATOR("%", Modulo);
        BIND_BINARY_OPERATOR("&", BitwiseAnd);
        BIND_BINARY_OPERATOR("|", BitwiseOr);
        BIND_BINARY_OPERATOR("^", BitwiseXor);
        BIND_BINARY_OPERATOR("<<", BitwiseShiftLeft);
        BIND_BINARY_OPERATOR(">>", BitwiseShiftRight);
        BIND_BINARY_OPERATOR("&&", LogicalAnd);
        BIND_BINARY_OPERATOR("||", LogicalOr);
        BIND_BINARY_OPERATOR("==", Equals);
        BIND_BINARY_OPERATOR("!=", NotEquals);
        BIND_BINARY_OPERATOR("<", LessThan);
        BIND_BINARY_OPERATOR(">", GreaterThan);
        BIND_BINARY_OPERATOR("<=", LessThanOrEqual);
        BIND_BINARY_OPERATOR(">=", GreaterThanOrEqual);

        SCC_NOT_IMPLEMENTED(op);

        #undef BIND_BINARY_OPERATOR

        SCC_BINDER_RESULT_TYPE(bind_binary_expression);
        return binding::BinderResult<ResultType>::error(binding::BinderError(ErrorKind::ReachedUnreachableError, node));

    }

}