#include "binding/binder.hpp"

namespace scc
{
    binding::BinderResult<binding::BoundLiteralExpression> Binder::bind_literal_expression(const TreeNode &node)
    {
        if (node.symbol() == Parser::NUMBER_LITERAL_SYMBOL)
        {
            return bind_number_literal(node);
        }
        else if (node.symbol() == Parser::STRING_LITERAL_SYMBOL)
        {
            // TODOOO: 
            // it has quotes, so we need to remove them
            // return std::make_unique<binding::BoundLiteralExpression>(node.value().substr(1, node.value().size() - 2),
            SCC_UNIMPLEMENTED();
        }
        else if (node.symbol() == Parser::CHAR_LITERAL_SYMBOL)
        {
            char c_value = 0;
            std::string node_value = node.value();
            if (node_value[1] == '\\')
            {
                switch (node_value[2])
                {
                case 'n':
                    c_value = '\n';
                    break;
                case 't':
                    c_value = '\t';
                    break;
                case 'r':
                    c_value = '\r';
                    break;
                case '0':
                    c_value = '\0';
                    break;
                case '\\':
                    c_value = '\\';
                    break;
                case '\'':
                    c_value = '\'';
                    break;
                case '\"':
                    c_value = '\"';
                    break;
                default:
                    SCC_UNREACHABLE();
                }
            }
            else
            {
                c_value = node_value[1];
            }
            return std::make_unique<binding::BoundLiteralExpression>(static_cast<Type::Primitive::Char>(c_value));
        }

        
        SCC_UNREACHABLE();
        
        SCC_BINDER_RESULT_TYPE(bind_literal_expression);  
        return binding::BinderResult<ResultType>::error(binding::BinderError(ErrorKind::ReachedUnreachableError, node));
      
    }
}