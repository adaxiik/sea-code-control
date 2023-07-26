#include "binding/binder.hpp"

#include "parser.hpp"
#include "debug.hpp"

#include <utility>
#include <iostream>
#include <iomanip>
#include <algorithm>

#define BUBBLE_ERROR(x) \
    if (!(x))           \
    return nullptr
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

namespace scc
{
    std::unique_ptr<binding::BoundBlockStatement> Binder::bind_block_statement(const TreeNode &node)
    {
        // SCC_ASSERT_NODE_SYMBOL(Parser::TRANSLATION_UNIT_SYMBOL);
        if (node.symbol() != Parser::TRANSLATION_UNIT_SYMBOL && node.symbol() != Parser::COMPOUND_STATEMENT_SYMBOL)
            return nullptr;

        auto block_statement = std::make_unique<binding::BoundBlockStatement>();
        for (uint32_t i = 0; i < node.named_child_count(); i++)
        {
            auto child = node.named_child(i);
            auto binded = bind_impl(child);
            BUBBLE_ERROR(binded);

            if(!binded->is_statement())
            {
                // TODOOO: UNREACHABLE???
                SCC_UNREACHABLE();                
            }
            auto bound_statement_ptr = static_cast<binding::BoundStatement*>(binded.release());
            block_statement->statements.push_back(std::unique_ptr<binding::BoundStatement>(bound_statement_ptr));
        }
        return block_statement;
    }

    std::unique_ptr<binding::BoundLiteralExpression> Binder::bind_number_literal(const TreeNode &node)
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
                return std::make_unique<binding::BoundLiteralExpression>(std::stoi(value, nullptr, base_int),
                                                                         Type::Kind::I32);
            case NumberType::LONG:
                return std::make_unique<binding::BoundLiteralExpression>(std::stoll(value, nullptr, base_int),
                                                                         Type::Kind::I64);
            case NumberType::LONG_LONG:
                return std::make_unique<binding::BoundLiteralExpression>(std::stoll(value, nullptr, base_int),
                                                                         Type::Kind::I64);
            case NumberType::UNSIGNED:
                return std::make_unique<binding::BoundLiteralExpression>(static_cast<uint32_t>(std::stoul(value, nullptr, base_int)),
                                                                         Type::Kind::U32);
            case NumberType::UNSIGNED_LONG:
                return std::make_unique<binding::BoundLiteralExpression>(std::stoull(value, nullptr, base_int),
                                                                         Type::Kind::U64);
            case NumberType::UNSIGNED_LONG_LONG:
                return std::make_unique<binding::BoundLiteralExpression>(std::stoull(value, nullptr, base_int),
                                                                         Type::Kind::U64);
            case NumberType::FLOAT:
                return std::make_unique<binding::BoundLiteralExpression>(std::stof(value, nullptr),
                                                                         Type::Kind::F32);
            case NumberType::DOUBLE:
                return std::make_unique<binding::BoundLiteralExpression>(std::stod(value, nullptr),
                                                                         Type::Kind::F64);
            default:
                SCC_UNREACHABLE();
            }
        }
        catch (const std::exception &e)
        {
            std::cerr << "Binder::bind_number_literal: " << e.what() << std::endl;
            return nullptr;
        }

        SCC_UNREACHABLE();
        return nullptr;
    }

    std::unique_ptr<binding::BoundLiteralExpression> Binder::bind_literal_expression(const TreeNode &node)
    {
        if (node.symbol() == Parser::NUMBER_LITERAL_SYMBOL)
        {
            return bind_number_literal(node);
        }
        else if (node.symbol() == Parser::STRING_LITERAL_SYMBOL)
        {
            // it has quotes, so we need to remove them
            // return std::make_unique<binding::BoundLiteralExpression>(node.value().substr(1, node.value().size() - 2),
            /// TODOOO);
            SCC_UNIMPLEMENTED();
        }
        else if (node.symbol() == Parser::CHAR_LITERAL_SYMBOL)
        {
            return std::make_unique<binding::BoundLiteralExpression>(node.value()[1], Type::Kind::Char);
        }
        else
        {
            SCC_UNREACHABLE();
        }
        return nullptr;
    }

    std::unique_ptr<binding::BoundExpression> Binder::bind_expression(const TreeNode &node)
    {
        // SCC_UNIMPLEMENTED();
        switch (node.symbol())
        {
        case Parser::BINARY_EXPRESSION_SYMBOL:
            return bind_binary_expression(node);
        case Parser::NUMBER_LITERAL_SYMBOL:
        case Parser::STRING_LITERAL_SYMBOL:
        case Parser::CHAR_LITERAL_SYMBOL:
            return bind_literal_expression(node);

        default:
            SCC_NOT_IMPLEMENTED_WARN(node.symbol_name());
            break;
        }
        return nullptr;
    }

    std::unique_ptr<binding::BoundBinaryExpression> Binder::bind_binary_expression(const TreeNode &node)
    {
        SCC_ASSERT_NODE_SYMBOL(Parser::BINARY_EXPRESSION_SYMBOL);
        SCC_ASSERT_CHILD_COUNT(node, 3);

        auto bound_left = bind_expression(node.child(0));
        BUBBLE_ERROR(bound_left);
        auto bound_right = bind_expression(node.child(2));
        BUBBLE_ERROR(bound_right);

        std::string op = node.child(1).value();

        #define BIND_BINARY_OPERATOR(x, y) \
            if (op == (x)){ \
                return std::make_unique<binding::BoundBinaryExpression>(std::move(bound_left), std::move(bound_right), binding::BoundBinaryExpression::OpKind::y); \
            }

        BIND_BINARY_OPERATOR("+", Addition);
        BIND_BINARY_OPERATOR("-", Subtraction);

        SCC_NOT_IMPLEMENTED(op);

        #undef BIND_BINARY_OPERATOR

        return nullptr;
    }

    std::unique_ptr<binding::BoundExpressionStatement> Binder::bind_expression_statement(const TreeNode &node)
    {
        SCC_ASSERT_NODE_SYMBOL(Parser::EXPRESSION_STATEMENT_SYMBOL);
        if (node.named_child_count() == 0) // eg.: `;`
            return nullptr;

        SCC_ASSERT_NAMED_CHILD_COUNT(node, 1);
        auto expression_statement = std::make_unique<binding::BoundExpressionStatement>();

        auto expression = bind_expression(node.named_child(0));
        BUBBLE_ERROR(expression);

        expression_statement->expression = std::move(expression);
        return expression_statement;
    }

    std::unique_ptr<binding::BoundNode> Binder::bind_impl(const TreeNode &node)
    {
        switch (node.symbol())
        {
        case Parser::TRANSLATION_UNIT_SYMBOL:
            return bind_block_statement(node);
        case Parser::EXPRESSION_STATEMENT_SYMBOL:
            return bind_expression_statement(node);
        case Parser::COMPOUND_STATEMENT_SYMBOL:
            return bind_block_statement(node);
        default:
            std::cerr << "Binder::bind_impl: Unhandled symbol: " << std::quoted(node.symbol_name()) << std::endl;
            return nullptr;
        }
    }

    std::unique_ptr<binding::BoundNode> Binder::bind(const TreeNode &node)
    {
        if (node.symbol() != Parser::TRANSLATION_UNIT_SYMBOL)
            return nullptr;

        return bind_impl(node);
    }
}