#include "binding/binder.hpp"

namespace scc
{
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
                return std::make_unique<binding::BoundLiteralExpression>(static_cast<Type::Primitive::I32>(std::stoll(value, nullptr, base_int)));
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

}