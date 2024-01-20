#include "binding/binder.hpp"

namespace scc
{
    binding::BinderResult<binding::BoundCallExpression> Binder::bind_call_expression(const TreeNode &node)
    {
        // call_expression ==>     fn(1,2,3)
        // ├── identifier ==>      fn
        // └── argument_list ==>   (1,2,3)
        //     ├── number_literal ==>      1
        //     ├── number_literal ==>      2
        //     └── number_literal ==>      3

        SCC_ASSERT_NODE_SYMBOL(Parser::CALL_EXPRESSION_SYMBOL);
        SCC_ASSERT_NAMED_CHILD_COUNT(node, 2);
        SCC_ASSERT_EQ(node.first_named_child().symbol(), Parser::IDENTIFIER_SYMBOL);
        SCC_BINDER_RESULT_TYPE(bind_call_expression);
        std::string function_name = node.first_named_child().value();
        if (m_functions.find(function_name) == m_functions.end())
        {
            auto error = binding::BinderResult<ResultType>(binding::BinderError(ErrorKind::UnknownSymbolError, node));
            error.add_diagnostic("Unknown function: " + function_name);
            return error;
        }

        FunctionDeclaration& fn_declaration = m_functions.at(function_name);
        if (fn_declaration.parameters.size() != node.named_child(1).named_child_count())
        {
            auto error = binding::BinderResult<ResultType>(binding::BinderError(ErrorKind::FunctionArgumentCountMismatchError, node));
            error.add_diagnostic("Invalid argument count for function: " + function_name);
            return error;
        }

        std::vector<std::unique_ptr<binding::BoundExpression>> arguments;
        for (size_t i = 0; i < node.named_child(1).named_child_count(); i++)
        {
            auto argument = bind_expression(node.named_child(1).named_child(i));
            BUBBLE_ERROR(argument);

            arguments.push_back(std::make_unique<binding::BoundCastExpression>(argument.release_value(), fn_declaration.parameters[i]));
        }

        return std::make_unique<binding::BoundCallExpression>(fn_declaration.return_type, std::move(function_name), std::move(arguments));
    }
}