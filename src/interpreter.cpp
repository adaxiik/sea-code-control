#include "interpreter.hpp"

#include <iostream>
#include "debug.hpp"

#if 0
    #define TRACE() std::cout << __PRETTY_FUNCTION__ << std::endl
#else
    #define TRACE()
#endif

// https://en.cppreference.com/w/cpp/utility/variant/visit
template<class... Ts>
struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;


namespace scc
{
    ParserResult Interpreter::parse(const std::string &code)
    {
        return m_parser.parse(code);
    }

    InterpreterResult Interpreter::interpret(const std::string &code)
    {
        auto parse_result = m_parser.parse(code);
        return interpret(parse_result);
    }

    InterpreterResult Interpreter::interpret(const ParserResult &parse_result)
    {
        if (parse_result.has_error())
            return InterpreterError::ParseError;

        auto binded {Binder::bind(parse_result.root_node())};
        if (!binded)
            return InterpreterError::BindError;

        if (binded->bound_node_kind() != binding::BoundNodeKind::BlockStatement)
            return InterpreterError::BindError;
        
        TRACE();
        return interpret(ikwid_rc<binding::BoundBlockStatement>(*binded));
    }

    InterpreterResult Interpreter::interpret(const binding::BoundBlockStatement &block_statement)
    {
        debug::bound_ast_as_text_tree(std::cout, block_statement);
        if(block_statement.statements.size() == 1)
            return interpret(*block_statement.statements[0]);

        for (const auto& statement : block_statement.statements)
        {
            auto result = interpret(*statement);
            if (result.is_error())
                return result;
        }

        return InterpreterError::None;
    }

    InterpreterResult Interpreter::interpret(const binding::BoundStatement &statement)
    {
        TRACE();
        static_assert(binding::STATEMENT_COUNT == 2, "Update this code");
        switch (statement.bound_node_kind())
        {
        case binding::BoundNodeKind::ExpressionStatement:
            return eval(*ikwid_rc<binding::BoundExpressionStatement>(statement).expression);
        case binding::BoundNodeKind::BlockStatement:
            return interpret(ikwid_rc<binding::BoundBlockStatement>(statement));
        default:
            // UNREACHABLE
            return InterpreterError::BindError;
        }
    }

    InterpreterResult Interpreter::eval(const binding::BoundExpression& expression)
    {
        TRACE();
        static_assert(binding::EXPRESSION_COUNT == 2, "Update this code");
        switch (expression.bound_node_kind())
        {
        case binding::BoundNodeKind::BinaryExpression:
            return eval(ikwid_rc<binding::BoundBinaryExpression>(expression));
        case binding::BoundNodeKind::LiteralExpression:
        {
            // return interpret(ikwid_rc<binding::BoundLiteralExpression>(expression));
            decltype(auto) literal = ikwid_rc<binding::BoundLiteralExpression>(expression);
            return InterpreterResult::ok(ResultValue(literal.value, literal.type));
        }
        default:
            // UNREACHABLE
            return InterpreterResult::error(InterpreterError::ReachedUnreachableError);
        }
    }
    
    InterpreterResult Interpreter::eval(const binding::BoundBinaryExpression& binary_expression)
    {
        TRACE();
        auto left_result{eval(*binary_expression.left)};
        if (left_result.is_error())
            return left_result;

        auto right_result{eval(*binary_expression.right)};
        if (right_result.is_error())
            return right_result;

        
        
        using Operator = binding::BoundBinaryExpression::OpKind;
        static_assert(static_cast<int>(Operator::COUNT) == 2, "Update this code");
        static_assert(static_cast<int>(Type::Kind::COUNT) == 12, "Update this code");

        // I'm so sorry
        #define DO_CASTED_OP(OP,LEFT_CAST,RIGHT_CAST) do{ \
            auto left {std::any_cast<LEFT_CAST>(left_result.get_value().value)}; \
            auto right {std::any_cast<RIGHT_CAST>(right_result.get_value().value)}; \
            return InterpreterResult::ok(ResultValue(static_cast<decltype(std::declval<LEFT_CAST>() + std::declval<RIGHT_CAST>())>(left OP right))); \
        }while(0)

        #define RIGHT_CASE(OP,LEFT_CAST,TYPE,CTYPE) case Type::Kind::TYPE: DO_CASTED_OP(OP,LEFT_CAST,CTYPE)

        #define DO_OP_RIGHT(OP, LEFT_CAST) do{                   \
            switch (right_result.get_value().type.kind)          \
            {                                                    \
                RIGHT_CASE(OP,LEFT_CAST,Char,char);              \
                RIGHT_CASE(OP,LEFT_CAST,U8,unsigned char);       \
                RIGHT_CASE(OP,LEFT_CAST,I8,signed char);         \
                RIGHT_CASE(OP,LEFT_CAST,U32,unsigned int);       \
                RIGHT_CASE(OP,LEFT_CAST,I32,int);                \
                RIGHT_CASE(OP,LEFT_CAST,U64,unsigned long long); \
                RIGHT_CASE(OP,LEFT_CAST,I64,long long);          \
                RIGHT_CASE(OP,LEFT_CAST,F32,float);              \
                RIGHT_CASE(OP,LEFT_CAST,F64,double);             \
                RIGHT_CASE(OP,LEFT_CAST,Bool,bool);              \
                default:                                         \
                    return InterpreterResult::error(InterpreterError::ReachedUnreachableError); \
            }                                                    \
        }while(0)

        #define LEFT_CASE(OP,TYPE,CTYPE) case Type::Kind::TYPE: DO_OP_RIGHT(OP,CTYPE)

        #define DO_OP_LEFT(OP) do{ \
            switch (left_result.get_value().type.kind) \
            { \
                LEFT_CASE(OP,Char,char); \
                LEFT_CASE(OP,U8,unsigned char); \
                LEFT_CASE(OP,I8,signed char); \
                LEFT_CASE(OP,U32,unsigned int); \
                LEFT_CASE(OP,I32,int); \
                LEFT_CASE(OP,U64,unsigned long long); \
                LEFT_CASE(OP,I64,long long); \
                LEFT_CASE(OP,F32,float); \
                LEFT_CASE(OP,F64,double); \
                LEFT_CASE(OP,Bool,bool); \
                default: \
                    return InterpreterResult::error(InterpreterError::ReachedUnreachableError); \
            } \
        }while(0)

        switch (binary_expression.op_kind)
        {
            case Operator::Addition:
                DO_OP_LEFT(+);
            case Operator::Subtraction:
                DO_OP_LEFT(-);
            default:
                // UNREACHABLE
                return InterpreterResult::error(InterpreterError::ReachedUnreachableError);
        }
        #undef DO_OP_LEFT
        #undef LEFT_CASE
        #undef DO_OP_RIGHT
        #undef RIGHT_CASE
        #undef DO_CASTED_OP


        return InterpreterResult::ok(1);
    }
}
