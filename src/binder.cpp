#include "binding/binder.hpp"

#include "parser.hpp"
#include "debug.hpp"

#include <utility>
#include <iostream>
#include <iomanip>
#include <algorithm>

        
#define SCC_ASSERT(x)                                                        \
    do                                                                       \
    {                                                                        \
        if (!(x))                                                            \
        {                                                                    \
            std::cerr << "==============================" << std::endl;      \
            std::cerr << "Assertion failed: " << #x << std::endl;            \
            std::cerr << "At: " << __FILE__ << ":" << __LINE__ << std::endl; \
            std::cerr << "==============================" << std::endl;      \
            std::exit(1);                                                    \
        }                                                                    \
    } while (0)
#define SCC_ASSERT_EQ(x, y) SCC_ASSERT((x) == (y))
#define SCC_ASSERT_NAMED_CHILD_COUNT(x, y) SCC_ASSERT_EQ((x).named_child_count(), (y))
#define SCC_ASSERT_CHILD_COUNT(x, y) SCC_ASSERT_EQ((x).child_count(), (y))
#define SCC_UNIMPLEMENTED()                                                         \
    do                                                                              \
    {                                                                               \
        std::cerr << "==============================" << std::endl;                 \
        std::cerr << "Unimplemented: " << __FILE__ << ":" << __LINE__ << std::endl; \
        std::cerr << "==============================" << std::endl;                 \
        std::exit(1);                                                               \
    } while (0)
#define SCC_ASSERT_NODE_SYMBOL(x) SCC_ASSERT_EQ(node.symbol(), (x))
#define SCC_NOT_IMPLEMENTED(x)                                           \
    do                                                                   \
    {                                                                    \
        std::cerr << "==============================" << std::endl;      \
        std::cerr << "Not implemented: " << std::quoted(x) << std::endl; \
        std::cerr << "At: " << __FILE__ << ":" << __LINE__ << std::endl; \
        std::cerr << "==============================" << std::endl;      \
        std::exit(1);                                                    \
    } while (0)
#define SCC_NOT_IMPLEMENTED_WARN(x)                                      \
    do                                                                   \
    {                                                                    \
        std::cerr << "==============================" << std::endl;      \
        std::cerr << "Not implemented: " << std::quoted(x) << std::endl; \
        std::cerr << "At: " << __FILE__ << ":" << __LINE__ << std::endl; \
        std::cerr << "==============================" << std::endl;      \
    } while (0)
#define SCC_UNREACHABLE() SCC_ASSERT(false)

#define SCC_BINDER_RESULT_TYPE(x) using ResultType = decltype(x(std::declval<const TreeNode&>()))::type

constexpr bool TRACE_BINDER = false;

// #define BUBBLE_ERROR(x) if ((x).is_error()) return (x)

#define SOURCE_LOCATION_STR std::string("at ") + std::string( __FILE__) + ":" + std::to_string(__LINE__)


#define BUBBLE_ERROR(x) do{ \
    if ((x).is_error()) \
    { \
        if constexpr (TRACE_BINDER) \
            (x).add_diagnostic(std::string(__func__) + " " + SOURCE_LOCATION_STR); \
        return (x); \
    } \
} while(0)


using ErrorKind = scc::binding::BinderError::BinderErrorKind;


#include <string>
namespace scc
{
    // std::unique_ptr<(\w*::\w*)> Binder
    // binding::BinderResult<$1> Binder

    binding::BinderResult<binding::BoundBlockStatement> Binder::bind_block_statement(const TreeNode &node)
    {
        // SCC_ASSERT_NODE_SYMBOL(Parser::TRANSLATION_UNIT_SYMBOL);
        SCC_BINDER_RESULT_TYPE(bind_block_statement);
        SCC_ASSERT(node.symbol() == Parser::TRANSLATION_UNIT_SYMBOL || node.symbol() == Parser::COMPOUND_STATEMENT_SYMBOL);

        auto block_statement = std::make_unique<binding::BoundBlockStatement>();
        if (node.symbol() == Parser::TRANSLATION_UNIT_SYMBOL && node.named_child_count() == 1)
        {
            // return bind_impl(node.first_named_child());
            auto result = bind_impl(node.first_named_child());
            BUBBLE_ERROR(result);
            if (!result.get_value()->is_statement())
            {
                SCC_UNREACHABLE();
            }

            auto statement = static_cast<binding::BoundStatement*>(result.release_value().release());
            block_statement->statements.push_back(std::unique_ptr<binding::BoundStatement>(statement));
            return binding::BinderResult<ResultType>::ok(std::move(block_statement));             
        }


        m_scope_stack.push();
        for (uint32_t i = 0; i < node.named_child_count(); i++)
        {
            auto child = node.named_child(i);
            if (child.symbol() == Parser::COMMENT_SYMBOL)
                continue;

            // ====================================================
            // TODOO: REMOVE THIS LATER!
            if (child.symbol() == Parser::PREPROC_INCLUDE_SYMBOL)
                continue;
            // ====================================================


            auto binded = bind_impl(child);
            BUBBLE_ERROR(binded);

            if(!binded.get_value()->is_statement())
            {
                // TODOOO: UNREACHABLE???
                SCC_UNREACHABLE();                
            }
            auto bound_statement_ptr = static_cast<binding::BoundStatement*>(binded.release_value().release());
            block_statement->statements.push_back(std::unique_ptr<binding::BoundStatement>(bound_statement_ptr));
        }
        m_scope_stack.pop();
        return binding::BinderResult<ResultType>::ok(std::move(block_statement));
    }

    binding::BinderResult<binding::BoundLiteralExpression> Binder::bind_number_literal(const TreeNode &node)
    {
        SCC_ASSERT_NODE_SYMBOL(Parser::NUMBER_LITERAL_SYMBOL);
        auto value = node.value();
        // supported:
        // prefixes: 0x, 0b, 0o
        // suffixes: u, l, ll, ul, ull

        enum class Base
        {
            DECIMAL = 10,
            BINARY = 2,
            OCTAL = 8,
            HEXADECIMAL = 16,
        };
        enum class NumberType
        {
            SIGNED,             // 4B - default
            LONG,               // 8B
            LONG_LONG,          // 8B
            UNSIGNED,           // 4B
            UNSIGNED_LONG,      // 8B
            UNSIGNED_LONG_LONG, // 8B
            FLOAT,              // 4B
            DOUBLE,             // 8B
        };

        Base base = Base::DECIMAL;
        NumberType number_type = NumberType::SIGNED;

        std::transform(value.begin(), value.end(), value.begin(), [](char c) -> char
                       { return std::tolower(c); });

        do
        {
            // 0. dec
            // 0 octal
            // 0b binary
            // 0x hexadecimal
            if (value.size() < 2)
                break;

            if (value.size() < 3 && value[0] == '0')
            {
                auto has_dot = value.find('.') != std::string::npos;
                if (has_dot)
                    break;

                base = Base::OCTAL;
                value = value.substr(1);
                break;
            }

            if (value.size() >= 3 && value[0] == '0')
            {
                if (value[1] == 'b')
                {
                    base = Base::BINARY;
                    value = value.substr(2);
                    break;
                }

                if (value[1] == 'x')
                {
                    base = Base::HEXADECIMAL;
                    value = value.substr(2);
                    break;
                }

                base = Base::OCTAL;
                value = value.substr(1);
            }

        } while (false);

        do
        {
            if (value.find('f') != std::string::npos 
            && value.find('.') != std::string::npos)
            {
                number_type = NumberType::FLOAT;
                break;
            }

            if (value.find('.') != std::string::npos)
            {
                number_type = NumberType::DOUBLE;
                break;
            }

            if (value.find("ull") != std::string::npos)
            {
                number_type = NumberType::UNSIGNED_LONG_LONG;
                break;
            }

            if (value.find("ul") != std::string::npos)
            {
                number_type = NumberType::UNSIGNED_LONG;
                break;
            }

            if (value.find('u') != std::string::npos)
            {
                number_type = NumberType::UNSIGNED;
                break;
            }

            if (value.find("ll") != std::string::npos)
            {
                number_type = NumberType::LONG_LONG;
                break;
            }

            if (value.find('l') != std::string::npos)
            {
                number_type = NumberType::LONG;
                break;
            }
        } while (false);

        auto base_int = static_cast<int>(base);
        try
        {
            switch (number_type)
            {
            case NumberType::SIGNED:
                return std::make_unique<binding::BoundLiteralExpression>(static_cast<Type::Primitive::I32>(std::stoi(value, nullptr, base_int)));
            case NumberType::LONG:
                return std::make_unique<binding::BoundLiteralExpression>(static_cast<Type::Primitive::I64>(std::stoll(value, nullptr, base_int)));
            case NumberType::LONG_LONG:
                return std::make_unique<binding::BoundLiteralExpression>(static_cast<Type::Primitive::I64>(std::stoll(value, nullptr, base_int)));
            case NumberType::UNSIGNED:
                return std::make_unique<binding::BoundLiteralExpression>(static_cast<Type::Primitive::U32>(std::stoul(value, nullptr, base_int)));
            case NumberType::UNSIGNED_LONG:
                return std::make_unique<binding::BoundLiteralExpression>(static_cast<Type::Primitive::U64>(std::stoull(value, nullptr, base_int)));
            case NumberType::UNSIGNED_LONG_LONG:
                return std::make_unique<binding::BoundLiteralExpression>(static_cast<Type::Primitive::U64>(std::stoull(value, nullptr, base_int)));
            case NumberType::FLOAT: 
                return std::make_unique<binding::BoundLiteralExpression>(static_cast<Type::Primitive::F32>(std::stof(value, nullptr)));
            case NumberType::DOUBLE:
                return std::make_unique<binding::BoundLiteralExpression>(static_cast<Type::Primitive::F64>(std::stod(value, nullptr)));

            default:
                SCC_UNREACHABLE();
            }
        }
        catch (const std::exception &e)
        {
            // std::cerr << "Binder::bind_number_literal: " << e.what() << std::endl;
            // return nullptr;
            SCC_BINDER_RESULT_TYPE(bind_number_literal);
            
            return binding::BinderResult<ResultType>::error(binding::BinderError(ErrorKind::InvalidNumberLiteralError, node));
            
        }

        SCC_UNREACHABLE();
        SCC_BINDER_RESULT_TYPE(bind_number_literal);
            
        return binding::BinderResult<ResultType>::error(binding::BinderError(ErrorKind::ReachedUnreachableError, node));
      
    }

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

    binding::BinderResult<binding::BoundIdentifierExpression> Binder::bind_identifier_expression(const TreeNode &node)
    {
        SCC_ASSERT_NODE_SYMBOL(Parser::IDENTIFIER_SYMBOL);
        SCC_ASSERT_CHILD_COUNT(node, 0);
        // TODOOOOOOO: might be a macro?
       
        Type* type = m_scope_stack.get_from_scopestack(node.value());
        if (!type)
        {
            SCC_BINDER_RESULT_TYPE(bind_identifier_expression);  
            auto error = binding::BinderResult<ResultType>(binding::BinderError(ErrorKind::UnknownSymbolError, node));
            error.add_diagnostic("Unknown identifier: " + node.value());
            return error;
        }
        return std::make_unique<binding::BoundIdentifierExpression>(node.value(), *type);
    }

    binding::BinderResult<binding::BoundAssignmentExpression> Binder::bind_assignment_expression(const TreeNode &node)
    {
        SCC_BINDER_RESULT_TYPE(bind_assignment_expression);  
        SCC_ASSERT_NODE_SYMBOL(Parser::ASSIGNMENT_EXPRESSION_SYMBOL);
        SCC_ASSERT_CHILD_COUNT(node, 3);
        // assignment_expression ==>       a+=9
        // ├── identifier ==>      a
        // ├── += ==>      +=
        // └── number_literal ==>  9
        // we will convert everything into a pure assignment expression
        // a += 9 ==> a = a + 9
        std::string op_kind = node.child(1).value();

        // TODOOO: int* a;
        // a = 5; // error .. to pointers I dont want to allow literal assignment
        // or we can check it in interpreter?

        const std::string identifier = node.first_child().value();
        Type* type_ptr = m_scope_stack.get_from_scopestack(identifier);
        if (!type_ptr)
        {
            auto error = binding::BinderResult<ResultType>(binding::BinderError(ErrorKind::UnknownSymbolError, node));
            error.add_diagnostic("Unknown identifier: " + identifier);
            return error;
        }
        Type type = *type_ptr;

        if (op_kind == "=")
        {
            auto expression = bind_expression(node.last_child());
            BUBBLE_ERROR(expression);
            auto casted_expression = std::make_unique<binding::BoundCastExpression>(expression.release_value(), type);
            return std::make_unique<binding::BoundAssignmentExpression>(std::move(identifier), type, std::move(casted_expression));
        }
        constexpr auto PLUS_ASSIGNMENT = "+=";
        constexpr auto MINUS_ASSIGNMENT = "-=";
        constexpr auto MULTIPLICATION_ASSIGNMENT = "*=";
        constexpr auto DIVISION_ASSIGNMENT = "/=";
        constexpr auto MODULO_ASSIGNMENT = "%=";
        constexpr auto BITWISE_AND_ASSIGNMENT = "&=";
        constexpr auto BITWISE_OR_ASSIGNMENT = "|=";
        constexpr auto BITWISE_XOR_ASSIGNMENT = "^=";
        constexpr auto BITWISE_SHIFT_LEFT_ASSIGNMENT = "<<=";
        constexpr auto BITWISE_SHIFT_RIGHT_ASSIGNMENT = ">>=";

        binding::BoundBinaryExpression::OperatorKind operator_kind;
        if (op_kind == PLUS_ASSIGNMENT)
            operator_kind = binding::BoundBinaryExpression::OperatorKind::Addition;
        else if (op_kind == MINUS_ASSIGNMENT)
            operator_kind = binding::BoundBinaryExpression::OperatorKind::Subtraction;
        else if (op_kind == MULTIPLICATION_ASSIGNMENT)
            operator_kind = binding::BoundBinaryExpression::OperatorKind::Multiplication;
        else if (op_kind == DIVISION_ASSIGNMENT)
            operator_kind = binding::BoundBinaryExpression::OperatorKind::Division;
        else if (op_kind == MODULO_ASSIGNMENT)
            operator_kind = binding::BoundBinaryExpression::OperatorKind::Modulo;
        else if (op_kind == BITWISE_AND_ASSIGNMENT)
            operator_kind = binding::BoundBinaryExpression::OperatorKind::BitwiseAnd;
        else if (op_kind == BITWISE_OR_ASSIGNMENT)
            operator_kind = binding::BoundBinaryExpression::OperatorKind::BitwiseOr;
        else if (op_kind == BITWISE_XOR_ASSIGNMENT)
            operator_kind = binding::BoundBinaryExpression::OperatorKind::BitwiseXor;
        else if (op_kind == BITWISE_SHIFT_LEFT_ASSIGNMENT)
            operator_kind = binding::BoundBinaryExpression::OperatorKind::BitwiseShiftLeft;
        else if (op_kind == BITWISE_SHIFT_RIGHT_ASSIGNMENT)
            operator_kind = binding::BoundBinaryExpression::OperatorKind::BitwiseShiftRight;
        else
            return binding::BinderResult<ResultType>::error(binding::BinderError(ErrorKind::ReachedUnreachableError, node));
                    
        auto expression = bind_expression(node.last_child());
        BUBBLE_ERROR(expression);
        auto binary_expression = std::make_unique<binding::BoundBinaryExpression>(std::make_unique<binding::BoundIdentifierExpression>(identifier, type)
                                                                                , expression.release_value()
                                                                                , type
                                                                                , operator_kind);
        auto casted_expression = std::make_unique<binding::BoundCastExpression>(std::move(binary_expression), type);

        return std::make_unique<binding::BoundAssignmentExpression>(std::move(identifier)
                                                                  , type
                                                                  , std::move(casted_expression));

    }

    binding::BinderResult<binding::BoundExpression> Binder::bind_expression(const TreeNode &node)
    {
        // SCC_UNIMPLEMENTED();
        switch (node.symbol())
        {
        case Parser::BINARY_EXPRESSION_SYMBOL:
            return bind_binary_expression(node);
        case Parser::CAST_EXPRESSION_SYMBOL:
            return bind_cast_expression(node);
        case Parser::PARENTHESIZED_EXPRESSION_SYMBOL:
            return bind_parenthesized_expression(node);
        case Parser::NUMBER_LITERAL_SYMBOL:
        case Parser::STRING_LITERAL_SYMBOL:
        case Parser::CHAR_LITERAL_SYMBOL:
            return bind_literal_expression(node);
        case Parser::IDENTIFIER_SYMBOL:
            return bind_identifier_expression(node);
        case Parser::ASSIGNMENT_EXPRESSION_SYMBOL:
            return bind_assignment_expression(node);
        case Parser::CALL_EXPRESSION_SYMBOL:
            return bind_call_expression(node);
        default:
            SCC_NOT_IMPLEMENTED_WARN(node.symbol_name());
            break;
        }

        SCC_BINDER_RESULT_TYPE(bind_expression);  
        return binding::BinderResult<ResultType>::error(binding::BinderError(ErrorKind::ReachedUnreachableError, node));

    }

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
                if(!deduced_type.has_value()){ \
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

    binding::BinderResult<binding::BoundCastExpression> Binder::bind_cast_expression(const TreeNode &node)
    {
        // cast_expression ==>     (float)1
        // ├── type_descriptor ==> float
        // │   └── primitive_type ==>      float
        // └── number_literal ==>  1

        SCC_ASSERT_NODE_SYMBOL(Parser::CAST_EXPRESSION_SYMBOL);
        SCC_ASSERT_NAMED_CHILD_COUNT(node, 2);

        auto bound_expression = bind_expression(node.named_child(1));
        BUBBLE_ERROR(bound_expression);

        auto type_descriptor = node.first_named_child();
        SCC_ASSERT_EQ(type_descriptor.symbol(), Parser::TYPE_DESCRIPTOR_SYMBOL);
        // SCC_ASSERT_CHILD_COUNT(type_descriptor, 1); // might be a pointer

        std::string type_name = type_descriptor.first_named_child().value();
        uint32_t pointer_depth = 0;
        if (type_descriptor.named_child_count() > 1)
        {
            auto current_node = type_descriptor.named_child(1);
            while (true)
            {
                pointer_depth++;
                if (current_node.named_child_count() < 1)
                    break;

                current_node = current_node.named_child(0);
            }
        }

        auto type = Type::from_string(type_name);
        if (!type.has_value())
        {
            // std::cerr << "Unknown type: " << type_name << std::endl;
            // return nullptr;
            SCC_BINDER_RESULT_TYPE(bind_cast_expression);
            auto error = binding::BinderResult<ResultType>(binding::BinderError(ErrorKind::UnknownSymbolError, type_descriptor));
            error.add_diagnostic("Unknown type: " + type_name);
            return error;
        }

        type.value().pointer_depth = pointer_depth;

        return std::make_unique<binding::BoundCastExpression>(bound_expression.release_value(), type.value());
    }

    binding::BinderResult<binding::BoundParenthesizedExpression> Binder::bind_parenthesized_expression(const TreeNode &node)
    {
        // parenthesized_expression ==>    (1,2,3)
        // └── comma_expression ==>        1,2,3
        //     ├── number_literal ==>      1
        //     └── comma_expression ==>    2,3
        //         ├── number_literal ==>  2
        //         └── number_literal ==>  3

        // or

        // binary_expression ==>   2 * (3 + 3)
        // ├── number_literal ==>  2
        // └── parenthesized_expression ==>        (3 + 3)
        //     └── binary_expression ==>   3 + 3
        //         ├── number_literal ==>  3
        //         └── number_literal ==>  3

        SCC_ASSERT_NODE_SYMBOL(Parser::PARENTHESIZED_EXPRESSION_SYMBOL);
        SCC_ASSERT_NAMED_CHILD_COUNT(node, 1);
        std::vector<std::unique_ptr<binding::BoundExpression>> expressions;
        if (node.first_named_child().symbol() == Parser::COMMA_EXPRESSION_SYMBOL)
        {
            auto current_node = node.first_named_child();

            do
            {
                auto expression = bind_expression(current_node.first_named_child());
                BUBBLE_ERROR(expression);
                expressions.push_back(expression.release_value());
                current_node = current_node.last_named_child();
            
            } while (current_node.symbol() == Parser::COMMA_EXPRESSION_SYMBOL);

            auto last_expression = bind_expression(current_node);
            BUBBLE_ERROR(last_expression);
            expressions.push_back(last_expression.release_value());
            
            return std::make_unique<binding::BoundParenthesizedExpression>(std::move(expressions));
        }
        else
        {
            auto expression = bind_expression(node.first_named_child());
            BUBBLE_ERROR(expression);

            return std::make_unique<binding::BoundParenthesizedExpression>(expression.release_value());
        }

    }

    binding::BinderResult<binding::BoundExpressionStatement> Binder::bind_expression_statement(const TreeNode &node)
    {
        SCC_ASSERT_NODE_SYMBOL(Parser::EXPRESSION_STATEMENT_SYMBOL);
        SCC_BINDER_RESULT_TYPE(bind_expression_statement);  
        
        if (node.named_child_count() == 0) // eg.: `;`
            return binding::BinderResult<ResultType>::error(binding::BinderError(ErrorKind::EmptyExpressionError, node));

        SCC_ASSERT_NAMED_CHILD_COUNT(node, 1);
        auto expression_statement = std::make_unique<binding::BoundExpressionStatement>();

        auto expression = bind_expression(node.named_child(0));
        BUBBLE_ERROR(expression);

        expression_statement->expression = expression.release_value();
        return  binding::BinderResult<ResultType>::ok(std::move(expression_statement));
    }

    binding::BinderResult<binding::BoundVariableDeclarationStatement> Binder::bind_variable_declaration(const TreeNode &node)
    {
        SCC_BINDER_RESULT_TYPE(bind_variable_declaration);

        // translation_unit ==>    int a;
        // └── declaration ==>     int a;
        //     ├── primitive_type ==>      int
        //     └── identifier ==>  a

        SCC_ASSERT(node.symbol() == Parser::DECLARATION_SYMBOL 
                || node.symbol() == Parser::PARAMETER_DECLARATION_SYMBOL);
        // 3 if have qualifier (eg.: `const`)
        // TODOOO: int a, b, c; 
        SCC_ASSERT(node.named_child_count() == 2 || node.named_child_count() == 3);


        bool has_qualifier = node.named_child_count() == 3;
        int type_index = static_cast<int>(has_qualifier);
        int identifier_index = type_index + 1;


        bool is_constant = has_qualifier && node.first_named_child().value() == "const";
        bool is_global = m_scope_stack.is_global_scope();
        auto type_descriptor = node.named_child(type_index).value();
        auto type = Type::from_string(type_descriptor);
        if(!type.has_value())
        {
            // TODOOOOOOOOOOOOO: custom types
            // std::cerr << "Unknown type: " << type_descriptor << std::endl;
            // return nullptr;
            SCC_BINDER_RESULT_TYPE(bind_variable_declaration);
            auto error = binding::BinderResult<ResultType>(binding::BinderError(ErrorKind::UnknownSymbolError, node));
            error.add_diagnostic("Unknown type: " + type_descriptor);
            return error;
        }


        // TODOOO: Investigate further mixing of pointer and array declarators.. and also with initializer
        // multidimensional arrays and multidimensional initializer..
        auto& type_scope = m_scope_stack;
        auto resolve_declarator = [&type, is_constant, &type_scope, is_global](const TreeNode &node, int identifier_index, bool has_initializer = false)
        ->  binding::BinderResult<binding::BoundVariableDeclarationStatement>
        {
            switch (node.named_child(identifier_index).symbol())
            {
                case Parser::IDENTIFIER_SYMBOL:
                {
                    std::string identifier = node.named_child(identifier_index).value();
                    if(!type_scope.create_variable(identifier, type.value()))
                        return binding::BinderResult<ResultType>::error(binding::BinderError(ErrorKind::FailedToCreateVariableError, node));

                    return static_cast<std::unique_ptr<binding::BoundVariableDeclarationStatement>>(
                         std::make_unique<binding::BoundVariableValueDeclarationStatement>(identifier
                    , type.value()
                    , is_constant
                    , is_global));
                    break;
                }
                case Parser::ARRAY_DECLARATOR_SYMBOL:
                {
                    // declaration ==>     int x[1];
                    // ├── primitive_type ==>      int
                    // └── array_declarator ==>    x[1]
                    //     ├── identifier ==>      x
                    //     └── number_literal ==>  1    //might be an identifier too 

                   
                    // TODOO: int x[1][2];  multi dimensional arrays

                    // int x[]; is error
                    // int x[] = {1,2,3}; is ok and this node wont have a number_literal child
                    if (node.named_child(identifier_index).named_child_count() != 2
                        && !has_initializer)
                        return static_cast<std::unique_ptr<binding::BoundVariableDeclarationStatement>>(
                            std::unique_ptr<binding::BoundVariableStaticArrayDeclarationStatement>(nullptr));

                    bool we_know_the_size = node.named_child(identifier_index).named_child_count() == 2;

                    if(we_know_the_size)
                    {
                        if (node.named_child(identifier_index).named_child(1).symbol() != Parser::NUMBER_LITERAL_SYMBOL)
                        {
                            // TODOOOOOOO: in int x[y]; y must be number_literal or macro identifier.. we dont want to support vla?
                            SCC_NOT_IMPLEMENTED("vla");
                        }
                    }
                
                    // now we know its a number, or unknown size
                    long long array_size = we_know_the_size ? std::stoll(node.named_child(identifier_index).named_child(1).value()) : 0;

                    SCC_ASSERT_EQ(node.named_child(identifier_index).first_named_child().symbol(), Parser::IDENTIFIER_SYMBOL);
                    std::string identifier = node.named_child(identifier_index).first_named_child().value();
                    type.value().pointer_depth = 1;

                    if(!type_scope.create_variable(identifier, type.value()))
                        return binding::BinderResult<ResultType>::error(binding::BinderError(ErrorKind::FailedToCreateVariableError, node));

                    return static_cast<std::unique_ptr<binding::BoundVariableDeclarationStatement>>(std::make_unique<binding::BoundVariableStaticArrayDeclarationStatement>(identifier
                    , type.value()
                    , array_size
                    , is_constant
                    , is_global));
                    break;
                }
                case Parser::POINTER_DECLARATOR_SYMBOL:
                {
                    auto current_node = node.named_child(identifier_index);
                    uint32_t pointer_depth = 0;
                    while (current_node.symbol() == Parser::POINTER_DECLARATOR_SYMBOL)
                    {
                        pointer_depth++;
                        current_node = current_node.first_named_child();
                    }

                    std::string identifier = current_node.value();
                    type.value().pointer_depth = pointer_depth;

                    if(!type_scope.create_variable(identifier, type.value()))
                        return binding::BinderResult<ResultType>::error(binding::BinderError(ErrorKind::FailedToCreateVariableError, node));

                    return  static_cast<std::unique_ptr<binding::BoundVariableDeclarationStatement>>(std::make_unique<binding::BoundVariablePointerDeclarationStatement>(identifier
                    , type.value()
                    , is_constant
                    , is_global));

                    break;
                }
                default:
                    std::cerr << "Unknown identifier symbol: " << node.named_child(identifier_index).symbol_name() << std::endl;
                    SCC_UNREACHABLE();
            }
        };

        switch (node.named_child(identifier_index).symbol())
        {
            case Parser::IDENTIFIER_SYMBOL:
            case Parser::ARRAY_DECLARATOR_SYMBOL:
            case Parser::POINTER_DECLARATOR_SYMBOL:
                return resolve_declarator(node, identifier_index);
            case Parser::INIT_DECLARATOR_SYMBOL:
            {
                // declaration ==>     int a[] = ...
                // ├── primitive_type ==>      int
                // └── init_declarator ==>     a[] = {1,2...
                //     ├── array_declarator ==>        a[]
                //     │   └── identifier ==>  a
                //     └── initializer_list ==>        {1,2}
                //         ├── number_literal ==>      1
                //         └── number_literal ==>      2

                // declaration ==>     int a = 0;
                // ├── primitive_type ==>      int
                // └── init_declarator ==>     a = 0
                //     ├── identifier ==>      a
                //     └── number_literal ==>  0

                auto init_declarator_node = node.named_child(identifier_index);
                // first is declaration and second is initializer
                auto declaration = resolve_declarator(init_declarator_node, 0, true);
                BUBBLE_ERROR(declaration);
                auto initializer_node = init_declarator_node.named_child(1);
                if (initializer_node.symbol() == Parser::INITIALIZER_LIST_SYMBOL)
                {                
     
                    switch (declaration.get_value()->variable_declaration_statement_kind())
                    {
                        using DeclarationKind = binding::BoundVariableDeclarationStatement::VariableDeclarationStatementKind;
                        case DeclarationKind::Value:
                        {
                            // int x = {1}; is ok, but int x = {1,2}; is not
                            // int x = {}; is not valid C99
                            if (initializer_node.named_child_count() != 1 )
                                return binding::BinderResult<ResultType>(binding::BinderError(ErrorKind::InvalidInitializerError, initializer_node));

                            auto initializer = bind_expression(initializer_node.first_named_child());
                            BUBBLE_ERROR(initializer);
                            auto casted_initializer = std::make_unique<binding::BoundCastExpression>(initializer.release_value(), type.value());

                            static_cast<binding::BoundVariableValueDeclarationStatement*>(declaration.get_value())->initializer = std::move(casted_initializer);
                            return declaration;
                        }
                        case DeclarationKind::Pointer:
                        {
                            if (initializer_node.named_child_count() != 1 )
                                return binding::BinderResult<ResultType>(binding::BinderError(ErrorKind::InvalidInitializerError, initializer_node));

                            
                            auto initializer = bind_expression(initializer_node.first_named_child());
                            BUBBLE_ERROR(initializer);
                            static_cast<binding::BoundVariablePointerDeclarationStatement*>(declaration.get_value())->initializer = initializer.release_value();
                            return declaration;
                        }
                        case DeclarationKind::StaticArray:
                        {
                                // int x[2] = {}; is not valid C99
                            if (initializer_node.named_child_count() == 0)
                                return binding::BinderResult<ResultType>(binding::BinderError(ErrorKind::InvalidInitializerError, initializer_node)); 

                            auto array_declaration = static_cast<binding::BoundVariableStaticArrayDeclarationStatement*>(declaration.get_value());
                            bool forced_size = array_declaration->array_size != 0;
                            if (forced_size && initializer_node.named_child_count() > array_declaration->array_size)
                            {
                                // initializer list is bigger than array size
                                // int x[2] = {1,2,3}; 
                                // but 
                                // int x[10] = {1,2,3}; is ok, and rest of the elements are 0
                                return binding::BinderResult<ResultType>(binding::BinderError(ErrorKind::InvalidInitializerError, initializer_node));
                            }

                            if (!forced_size)
                                array_declaration->array_size = initializer_node.named_child_count();
                            
                            for (uint32_t i = 0; i < array_declaration->array_size; i++)
                            {
                                if (i < initializer_node.named_child_count())
                                {
                                    auto initializer = bind_expression(initializer_node.named_child(i));
                                    BUBBLE_ERROR(initializer);
                                    array_declaration->initializers.push_back(initializer.release_value());
                                }
                                else
                                {
                                    // TODOOOOO: what if its custom type? investigate or @help

                                    // rest of the elements are 0
                                    // auto initializer = 
                                    // BUBBLE_ERROR(initializer);
                                    // declaration->initializers.push_back(std::move(initializer));
                                    SCC_UNIMPLEMENTED();
                                }
                            }

                            return declaration;
                        }
                        default:
                            SCC_UNREACHABLE();
                    }
                    
                }
                
                // probably just an expression?
                // TODOOO: investigate other initializers
                using DeclarationKind = binding::BoundVariableDeclarationStatement::VariableDeclarationStatementKind;
                static_assert(static_cast<int>(DeclarationKind::COUNT) == 3, "Update this switch");
                switch (declaration.get_value()->variable_declaration_statement_kind())
                {
                    case DeclarationKind::Value:
                    {
                        auto initializer = bind_expression(initializer_node);
                        BUBBLE_ERROR(initializer);
                        auto casted_initializer = std::make_unique<binding::BoundCastExpression>(initializer.release_value(), type.value());
                        static_cast<binding::BoundVariableValueDeclarationStatement*>(declaration.get_value())->initializer = std::move(casted_initializer);
                        return declaration;
                    }
                    case DeclarationKind::Pointer:
                    {
                        auto initializer = bind_expression(initializer_node);
                        BUBBLE_ERROR(initializer);
                        static_cast<binding::BoundVariablePointerDeclarationStatement*>(declaration.get_value())->initializer = initializer.release_value();
                        return declaration;
                    }
                    case DeclarationKind::StaticArray:
                    {
                        // int x[] = 1;
                        // int x[2] = 1; 
                        // both are invalid
                        return binding::BinderResult<ResultType>(binding::BinderError(ErrorKind::InvalidInitializerError, initializer_node));
                    }
                    default:
                        SCC_UNREACHABLE();
                } 
            }
        }
        return binding::BinderResult<ResultType>(binding::BinderError(ErrorKind::ReachedUnreachableError, node));
    }

    binding::BinderResult<binding::BoundIfStatement> Binder::bind_if_statement(const TreeNode &node)
    {
        // if_statement ==>    if (true) ...
        // ├── parenthesized_expression ==>    (true)
        // │   └── true ==>    true
        // └── expression_statement ==>        a = 5;
        //     └── assignment_expression ==>   a = 5
        //         ├── identifier ==>  a
        //         └── number_literal ==>      5

        SCC_ASSERT_NODE_SYMBOL(Parser::IF_STATEMENT_SYMBOL);
        SCC_ASSERT(node.named_child_count() == 2 || node.named_child_count() == 3);
        bool has_else = node.named_child_count() == 3;
        auto condition_expr = bind_expression(node.first_named_child());
        BUBBLE_ERROR(condition_expr);
        
        // we will cast the condition to bool
        auto condition = std::make_unique<binding::BoundCastExpression>(condition_expr.release_value(), Type(Type::Kind::Bool));

        auto then_statement_result = bind_impl(node.named_child(1));
        BUBBLE_ERROR(then_statement_result);
        
        SCC_BINDER_RESULT_TYPE(bind_if_statement);
        if(!then_statement_result.get_value()->is_statement())
            return binding::BinderResult<ResultType>(binding::BinderError(ErrorKind::ReachedUnreachableError, node));

        std::unique_ptr<binding::BoundStatement> then_statement = std::unique_ptr<binding::BoundStatement>(static_cast<binding::BoundStatement*>(then_statement_result.release_value().release()));

        std::unique_ptr<binding::BoundStatement> else_statement = nullptr;
        if (has_else)
        {
            auto else_statement_result = bind_impl(node.named_child(2));
            BUBBLE_ERROR(else_statement_result);
            if(!else_statement_result.get_value()->is_statement())
                return binding::BinderResult<ResultType>(binding::BinderError(ErrorKind::ReachedUnreachableError, node));

            else_statement = std::unique_ptr<binding::BoundStatement>(static_cast<binding::BoundStatement*>(else_statement_result.release_value().release()));
        }

        return std::make_unique<binding::BoundIfStatement>(std::move(condition), std::move(then_statement), std::move(else_statement));
    }

    binding::BinderResult<binding::BoundWhileStatement> Binder::bind_while_statement(const TreeNode &node)
    {
        // while_statement ==> while (1) ...
        // ├── parenthesized_expression ==>    (1)
        // │   └── 1 ==>    1
        // └── expression_statement ==>        a = 5;
        //     └── assignment_expression ==>   a = 5
        //         ├── identifier ==>  a
        //         └── number_literal ==>      5

        SCC_ASSERT_NODE_SYMBOL(Parser::WHILE_STATEMENT_SYMBOL);
        SCC_ASSERT_NAMED_CHILD_COUNT(node, 2);
        auto condition_expr = bind_expression(node.first_named_child());
        BUBBLE_ERROR(condition_expr);
        
        // we will cast the condition to bool
        auto condition = std::make_unique<binding::BoundCastExpression>(condition_expr.release_value(), Type(Type::Kind::Bool));

        auto body_statement_result = bind_impl(node.named_child(1));
        BUBBLE_ERROR(body_statement_result);
        
        SCC_BINDER_RESULT_TYPE(bind_while_statement);
        if(!body_statement_result.get_value()->is_statement())
            return binding::BinderResult<ResultType>(binding::BinderError(ErrorKind::ReachedUnreachableError, node));

        std::unique_ptr<binding::BoundStatement> body_statement = std::unique_ptr<binding::BoundStatement>(static_cast<binding::BoundStatement*>(body_statement_result.release_value().release()));

        return std::make_unique<binding::BoundWhileStatement>(std::move(condition), std::move(body_statement));
    }

    binding::BinderResult<binding::BoundDoStatement> Binder::bind_do_statement(const TreeNode &node)
    {
        // do_statement ==>    do {int a ...
        // ├── compound_statement ==>  {int a = 5...
        // │   └── declaration ==>     int a = 5;
        // │       ├── primitive_type ==>      int
        // │       └── init_declarator ==>     a = 5
        // │           ├── identifier ==>      a
        // │           └── number_literal ==>  5
        // └── parenthesized_expression ==>    (0)
        //     └── number_literal ==>  0

        SCC_ASSERT_NODE_SYMBOL(Parser::DO_STATEMENT_SYMBOL);
        SCC_ASSERT_NAMED_CHILD_COUNT(node, 2);
        auto body_statement_result = bind_impl(node.first_named_child());
        BUBBLE_ERROR(body_statement_result);

        SCC_BINDER_RESULT_TYPE(bind_do_statement);

        if(!body_statement_result.get_value()->is_statement())
            return binding::BinderResult<ResultType>(binding::BinderError(ErrorKind::ReachedUnreachableError, node));

        std::unique_ptr<binding::BoundStatement> body_statement = std::unique_ptr<binding::BoundStatement>(static_cast<binding::BoundStatement*>(body_statement_result.release_value().release()));

        auto condition_expr = bind_expression(node.named_child(1));
        BUBBLE_ERROR(condition_expr);

        // we will cast the condition to bool
        auto condition = std::make_unique<binding::BoundCastExpression>(condition_expr.release_value(), Type(Type::Kind::Bool));

        return std::make_unique<binding::BoundDoStatement>(std::move(condition), std::move(body_statement));
    }

    binding::BinderResult<binding::BoundBreakStatement> Binder::bind_break_statement(const TreeNode &node)
    {
        SCC_ASSERT_NODE_SYMBOL(Parser::BREAK_STATEMENT_SYMBOL);
        SCC_ASSERT_NAMED_CHILD_COUNT(node, 0);
        return std::make_unique<binding::BoundBreakStatement>();
    }

    binding::BinderResult<binding::BoundContinueStatement> Binder::bind_continue_statement(const TreeNode &node)
    {
        SCC_ASSERT_NODE_SYMBOL(Parser::CONTINUE_STATEMENT_SYMBOL);
        SCC_ASSERT_NAMED_CHILD_COUNT(node, 0);
        return std::make_unique<binding::BoundContinueStatement>();
    }

    binding::BinderResult<binding::BoundFunctionStatement> Binder::bind_function(const TreeNode &node)
    {
        SCC_ASSERT(node.symbol() == Parser::FUNCTION_DEFINITION_SYMBOL ||
                   node.symbol() == Parser::DECLARATION_SYMBOL);
        SCC_BINDER_RESULT_TYPE(bind_function);

        // int fn(int a, int b) {}

        // function_definition ==>     int fn(int...
        // ├── primitive_type ==>      int
        // ├── function_declarator ==> fn(int a, ...
        // │   ├── identifier ==>      fn
        // │   └── parameter_list ==>  (int a, in...
        // │       ├── parameter_declaration ==>       int a
        // │       │   ├── primitive_type ==>  int
        // │       │   └── identifier ==>      a
        // │       └── parameter_declaration ==>       int b
        // │           ├── primitive_type ==>  int
        // │           └── identifier ==>      b
        // └── compound_statement ==>  {}

        // int fn(int a, int b);
        // declaration ==>     int fn(int...
        // ├── primitive_type ==>      int
        // └── function_declarator ==> fn(int a, ...
        //     ├── identifier ==>      fn
        //     └── parameter_list ==>  (int a, in...
        //         ├── parameter_declaration ==>       int a
        //         │   ├── primitive_type ==>  int
        //         │   └── identifier ==>      a
        //         └── parameter_declaration ==>       int b
        //             ├── primitive_type ==>  int
        //             └── identifier ==>      b

        // int** a();
        // declaration ==>     int** a();
        // ├── primitive_type ==>      int
        // └── pointer_declarator ==>  ** a()
        //     └── pointer_declarator ==>      * a()
        //         └── function_declarator ==> a()
        //             ├── identifier ==>      a
        //             └── parameter_list ==>  ()

        int type_index = node.first_named_child().symbol() == Parser::STORAGE_CLASS_SPECIFIER_SYMBOL; // eg.: `static`
        
        // TODOO: custom types
        std::optional<Type> return_type_opt = Type::from_string(node.named_child(type_index).value());
        if (!return_type_opt.has_value())
        {
            auto error = binding::BinderResult<ResultType>(binding::BinderError(ErrorKind::UnknownSymbolError, node));
            error.add_diagnostic("Unknown type: " + node.named_child(type_index).value());
            return error;
        }

        int declarator_index = type_index + 1;

        uint32_t pointer_depth = 0;
        if (node.named_child(declarator_index).symbol() == Parser::POINTER_DECLARATOR_SYMBOL)
        {
            auto current_node = node.named_child(declarator_index);
            while (current_node.symbol() == Parser::POINTER_DECLARATOR_SYMBOL)
            {
                pointer_depth++;
                current_node = current_node.first_named_child();
            }
        }

        auto return_type = return_type_opt.value();
        return_type.pointer_depth = pointer_depth;

        TreeNode declarator = node.named_child_with_symbol_BFS(Parser::FUNCTION_DECLARATOR_SYMBOL).value();

        std::string function_name = declarator.first_named_child().value();
        TreeNode parameter_list = declarator.named_child(1);
        
        
        std::vector<std::unique_ptr<binding::BoundVariableDeclarationStatement>> parameters;
        FunctionDeclaration fn_declaration = {return_type, std::vector<Type>(), false};
        
        m_scope_stack.push();
        for (size_t i = 0; i < parameter_list.named_child_count(); i++)
        {
            SCC_ASSERT(parameter_list.named_child(i).symbol() == Parser::PARAMETER_DECLARATION_SYMBOL);
            auto parameter_declaration = parameter_list.named_child(i);

            // TODOOO: It doesnt have to have a name, but idc for now
            auto bound_declaration = bind_variable_declaration(parameter_declaration);
            BUBBLE_ERROR(bound_declaration);
            m_scope_stack.create_variable(bound_declaration.get_value()->variable_name, bound_declaration.get_value()->type);
            fn_declaration.parameters.push_back(bound_declaration.get_value()->type);
            parameters.emplace_back(bound_declaration.release_value());
        }
        
        m_current_function = fn_declaration;
        if (m_functions.find(function_name) != m_functions.end())
        {
            auto& existing_fn_declaration = m_functions.at(function_name);
            if (existing_fn_declaration != fn_declaration)
            {
                auto error = binding::BinderResult<ResultType>(binding::BinderError(ErrorKind::FunctionSignatureMismatchError, node));
                error.add_diagnostic("Function signature mismatch: " + function_name);
                return error;
            }

            if (existing_fn_declaration.is_defined)
            {
                auto error = binding::BinderResult<ResultType>(binding::BinderError(ErrorKind::FunctionAlreadyDefinedError, node));
                error.add_diagnostic("Function redeclaration: " + function_name);
                return error;
            }
        }
        else
        {
            m_functions.insert({function_name, fn_declaration});
        }


        // TODOOOOOOOOO: check for missing return statement
        std::unique_ptr<binding::BoundBlockStatement> body = nullptr;
        if (node.last_named_child().symbol() == Parser::COMPOUND_STATEMENT_SYMBOL)
        {
            auto body_result = bind_impl(node.last_named_child());
            BUBBLE_ERROR(body_result);
            body = std::unique_ptr<binding::BoundBlockStatement>(static_cast<binding::BoundBlockStatement*>(body_result.release_value().release()));
            m_functions.at(function_name).is_defined = true;
        }
        m_scope_stack.pop();
        
       return std::make_unique<binding::BoundFunctionStatement>(return_type, std::move(function_name), std::move(parameters), std::move(body));
    }

    binding::BinderResult<binding::BoundNode> Binder::bind_declaration(const TreeNode &node)
    {
        SCC_ASSERT_NODE_SYMBOL(Parser::DECLARATION_SYMBOL);

        if (node.has_named_child_with_symbol(Parser::FUNCTION_DECLARATOR_SYMBOL))
            return bind_function(node);
        
        return bind_variable_declaration(node);
    }

    binding::BinderResult<binding::BoundReturnStatement> Binder::bind_return_statement(const TreeNode &node)
    {
        SCC_ASSERT_NODE_SYMBOL(Parser::RETURN_STATEMENT_SYMBOL);
        SCC_BINDER_RESULT_TYPE(bind_return_statement);
        if (!m_current_function.has_value())
            return binding::BinderResult<ResultType>(binding::BinderError(ErrorKind::ReturnStatementOutsideFunctionError, node));

        if (node.named_child_count() == 1)
        {
            auto expression = bind_expression(node.first_named_child());
            BUBBLE_ERROR(expression);

            return std::make_unique<binding::BoundReturnStatement>(std::make_unique<binding::BoundCastExpression>(expression.release_value()
                                                                , m_current_function.value().get().return_type));
        }

        if (m_current_function.value().get().return_type != Type(Type::Kind::Void))
        {
            auto error = binding::BinderResult<ResultType>(binding::BinderError(ErrorKind::ReturnStatementMissingExpressionError, node));
            return error;
        }

        return std::make_unique<binding::BoundReturnStatement>();
    }

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

    binding::BinderResult<binding::BoundNode> Binder::bind_impl(const TreeNode &node)
    {
        SCC_BINDER_RESULT_TYPE(bind_impl);

        switch (node.symbol())
        {
        case Parser::TRANSLATION_UNIT_SYMBOL:
            return bind_block_statement(node);
        case Parser::EXPRESSION_STATEMENT_SYMBOL:
            return bind_expression_statement(node);
        case Parser::COMPOUND_STATEMENT_SYMBOL:
            return bind_block_statement(node);
        case Parser::DECLARATION_SYMBOL:
            return bind_declaration(node);
        case Parser::IF_STATEMENT_SYMBOL:
            return bind_if_statement(node);
        case Parser::WHILE_STATEMENT_SYMBOL:
            return bind_while_statement(node);
        case Parser::DO_STATEMENT_SYMBOL:
            return bind_do_statement(node);
        case Parser::BREAK_STATEMENT_SYMBOL:
            return bind_break_statement(node);
        case Parser::CONTINUE_STATEMENT_SYMBOL:
            return bind_continue_statement(node);
        case Parser::FUNCTION_DEFINITION_SYMBOL:
            return bind_function(node);
        case Parser::RETURN_STATEMENT_SYMBOL:
            return bind_return_statement(node);
        default:
            std::cerr << "Binder::bind_impl: Unhandled symbol: " << std::quoted(node.symbol_name()) << std::endl;
            return binding::BinderResult<ResultType>(binding::BinderError(ErrorKind::ReachedUnreachableError, node));
        }
    }

    binding::BinderResult<binding::BoundNode> Binder::bind(const TreeNode &node)
    {
        SCC_BINDER_RESULT_TYPE(bind);
        if (node.symbol() != Parser::TRANSLATION_UNIT_SYMBOL)
            return binding::BinderResult<ResultType>(binding::BinderError(ErrorKind::ReachedUnreachableError, node));

        return bind_impl(node);
    }
}