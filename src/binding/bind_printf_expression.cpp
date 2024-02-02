#include "binding/binder.hpp"

namespace scc
{
    binding::BinderResult<binding::BoundPrintfExpression> Binder::bind_printf_expression(const TreeNode &node)
    {
        // call_expression ==>     fn(1,2,3)
        // ├── identifier ==>      fn
        // └── argument_list ==>   (1,2,3)
        //     ├── number_literal ==>      1
        //     ├── number_literal ==>      2
        //     └── number_literal ==>      3

        SCC_ASSERT_NODE_SYMBOL(Parser::CALL_EXPRESSION_SYMBOL);
        SCC_ASSERT_EQ(node.first_named_child().symbol(), Parser::IDENTIFIER_SYMBOL);
        SCC_BINDER_RESULT_TYPE(bind_call_expression);

        TreeNode argument_list = node.last_named_child();

        if (argument_list.named_child_count() == 0)
        {
            auto error = binding::BinderResult<ResultType>(binding::BinderError(ErrorKind::FunctionArgumentCountMismatchError, node));
            error.add_diagnostic("Printf requires at least one argument");
            return error;
        }

        std::vector<std::unique_ptr<binding::BoundExpression>> arguments;

        auto format_argument = bind_expression(argument_list.named_child(0));
        BUBBLE_ERROR(format_argument);

        if (format_argument.get_value()->type.primitive_type().value_or(Type::PrimitiveType::COUNT) != Type::PrimitiveType::Char
            and format_argument.get_value()->type.pointer_depth() != 1)
        {
            auto error = binding::BinderResult<ResultType>(binding::BinderError(ErrorKind::FunctionSignatureMismatchError, node));
            error.add_diagnostic("First argument of printf must be a string literal");
            return error;
        }



        for (size_t i = 1; i < argument_list.named_child_count(); i++)
        {
            auto argument = bind_expression(argument_list.named_child(i));
            BUBBLE_ERROR(argument);

            arguments.push_back(argument.release_value());
        }

        return std::make_unique<binding::BoundPrintfExpression>(format_argument.release_value(), std::move(arguments));
    }
}