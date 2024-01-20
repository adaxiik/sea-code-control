#include "binding/binder.hpp"

namespace scc
{
    binding::BinderResult<binding::BoundExpression> Binder::bind_identifier_expression(const TreeNode &node)
    {
        SCC_ASSERT_NODE_SYMBOL(Parser::IDENTIFIER_SYMBOL);
        SCC_ASSERT_CHILD_COUNT(node, 0);
        SCC_BINDER_RESULT_TYPE(bind_identifier_expression);  

        if (m_macros.find(node.value()) != m_macros.end())
        {
            const std::string& macro_value = m_macros[node.value()];
            Parser parser = Parser();
            ParserResult parser_result = parser.parse(macro_value, true);
            if (parser_result.has_error())
            {
                SCC_BINDER_RESULT_TYPE(bind_identifier_expression);  
                auto error = binding::BinderResult<ResultType>(binding::BinderError(ErrorKind::DefineMacroParseError, node));
                error.add_diagnostic("Failed to parse macro value: " + macro_value);
                return error;
            }
            auto binded = bind(parser_result.root_node());
            BUBBLE_ERROR(binded);
            auto translation_unit = std::unique_ptr<binding::BoundBlockStatement>(static_cast<binding::BoundBlockStatement*>(binded.release_value().release()));

            if (translation_unit->statements.front()->bound_node_kind() != binding::BoundNodeKind::ExpressionStatement)
                return binding::BinderResult<ResultType>::error(binding::BinderError(ErrorKind::DefineMacroUnsupportedError, node));
            
            auto expression_statement = std::unique_ptr<binding::BoundExpressionStatement>(static_cast<binding::BoundExpressionStatement*>(translation_unit->statements.front().release()));
            return std::move(expression_statement->expression);
        }


       
        Type* type = m_scope_stack.get_from_scopestack(node.value());
        if (not type)
        {
            auto error = binding::BinderResult<ResultType>(binding::BinderError(ErrorKind::UnknownSymbolError, node));
            error.add_diagnostic("Unknown identifier: " + node.value());
            return error;
        }

        auto bound_expression = std::make_unique<binding::BoundIdentifierExpression>(node.value(), *type);
        
        return std::unique_ptr<binding::BoundExpression>(std::move(bound_expression));
    }

}