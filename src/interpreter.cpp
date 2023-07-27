#include "interpreter.hpp"

#include <iostream>
#include "debug.hpp"
#include "cpp_compiler.hpp"
#include "operation_result.hpp"


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
        SCC_SUPPRESS_WARNING_PUSH_SIGN_COMPARISON


        TRACE();
        auto left_result{eval(*binary_expression.left)};
        if (left_result.is_error())
            return left_result;

        auto right_result{eval(*binary_expression.right)};
        if (right_result.is_error())
            return right_result;

        
        
        using Operator = binding::BoundBinaryExpression::OperatorKind;
        static_assert(static_cast<int>(Operator::COUNT) == 18, "Update this code");
        static_assert(static_cast<int>(Type::Kind::COUNT) == 12, "Update this code");

        // I'm so sorry
        
        #define DO_CASTED_OP(FN_NAME, STRUCT_NAME, LEFT_CAST, RIGHT_CAST) do{ \
           if constexpr (std::is_same_v<typename OperationResult::STRUCT_NAME ## OperationResult <LEFT_CAST,RIGHT_CAST>::type, OperationResult::InvalidOperation>) \
           { \
                return InterpreterResult::error(InterpreterError::InvalidOperationError); \
           } \
           else \
           { \
                return InterpreterResult::ok(ResultValue(OperationResult::perform_##FN_NAME(std::any_cast<LEFT_CAST>(left_result.get_value().value), std::any_cast<RIGHT_CAST>(right_result.get_value().value)))); \
           } \
        }while(0)

        #define RIGHT_CASE(OP,STRUCT_NAME,LEFT_CAST,TYPE,CTYPE) case Type::Kind::TYPE: DO_CASTED_OP(OP,STRUCT_NAME,LEFT_CAST,CTYPE)

        #define DO_OP_RIGHT(OP,STRUCT_NAME,LEFT_CAST) do{        \
            switch (right_result.get_value().type.kind)          \
            {                                                    \
                RIGHT_CASE(OP,STRUCT_NAME,LEFT_CAST,Char,char);              \
                RIGHT_CASE(OP,STRUCT_NAME,LEFT_CAST,U8,unsigned char);       \
                RIGHT_CASE(OP,STRUCT_NAME,LEFT_CAST,I8,signed char);         \
                RIGHT_CASE(OP,STRUCT_NAME,LEFT_CAST,U32,unsigned int);       \
                RIGHT_CASE(OP,STRUCT_NAME,LEFT_CAST,I32,int);                \
                RIGHT_CASE(OP,STRUCT_NAME,LEFT_CAST,U64,unsigned long long); \
                RIGHT_CASE(OP,STRUCT_NAME,LEFT_CAST,I64,long long);          \
                RIGHT_CASE(OP,STRUCT_NAME,LEFT_CAST,F32,float);              \
                RIGHT_CASE(OP,STRUCT_NAME,LEFT_CAST,F64,double);             \
                RIGHT_CASE(OP,STRUCT_NAME,LEFT_CAST,Bool,bool);              \
                default:                                         \
                    return InterpreterResult::error(InterpreterError::ReachedUnreachableError); \
            }                                                    \
        }while(0)

        #define LEFT_CASE(OP,STRUCT_NAME,TYPE,CTYPE) case Type::Kind::TYPE: DO_OP_RIGHT(OP,STRUCT_NAME,CTYPE)

        #define DO_OP_LEFT(OP,STRUCT_NAME) do{ \
            switch (left_result.get_value().type.kind) \
            { \
                LEFT_CASE(OP,STRUCT_NAME,Char,char); \
                LEFT_CASE(OP,STRUCT_NAME,U8,unsigned char); \
                LEFT_CASE(OP,STRUCT_NAME,I8,signed char); \
                LEFT_CASE(OP,STRUCT_NAME,U32,unsigned int); \
                LEFT_CASE(OP,STRUCT_NAME,I32,int); \
                LEFT_CASE(OP,STRUCT_NAME,U64,unsigned long long); \
                LEFT_CASE(OP,STRUCT_NAME,I64,long long); \
                LEFT_CASE(OP,STRUCT_NAME,F32,float); \
                LEFT_CASE(OP,STRUCT_NAME,F64,double); \
                LEFT_CASE(OP,STRUCT_NAME,Bool,bool); \
                default: \
                    return InterpreterResult::error(InterpreterError::ReachedUnreachableError); \
            } \
        }while(0)

        switch (binary_expression.op_kind)
        {
            case Operator::Addition: DO_OP_LEFT(addition,Addition);
            case Operator::Subtraction: DO_OP_LEFT(subtraction,Subtraction);
            case Operator::Multiplication: DO_OP_LEFT(multiplication,Multiplication);
            case Operator::Division: DO_OP_LEFT(division,Division);
            case Operator::Modulo: DO_OP_LEFT(modulo,Modulo);
            case Operator::BitwiseAnd: DO_OP_LEFT(bitwiseand,BitwiseAnd);
            case Operator::BitwiseOr: DO_OP_LEFT(bitwiseor,BitwiseOr);
            case Operator::BitwiseXor: DO_OP_LEFT(bitwisexor,BitwiseXor);
            case Operator::BitwiseShiftLeft: DO_OP_LEFT(bitwiseshiftleft,BitwiseShiftLeft);
            case Operator::BitwiseShiftRight: DO_OP_LEFT(bitwiseshiftright,BitwiseShiftRight);
            case Operator::LogicalAnd: DO_OP_LEFT(logicaland,LogicalAnd);
            case Operator::LogicalOr: DO_OP_LEFT(logicalor,LogicalOr);
            case Operator::Equals: DO_OP_LEFT(equals,Equals);
            case Operator::NotEquals: DO_OP_LEFT(notequals,NotEquals);
            case Operator::LessThan: DO_OP_LEFT(lessthan,LessThan);
            case Operator::GreaterThan: DO_OP_LEFT(greaterthan,GreaterThan);
            case Operator::LessThanOrEqual: DO_OP_LEFT(lessthanorequal,LessThanOrEqual);
            case Operator::GreaterThanOrEqual: DO_OP_LEFT(greaterthanorequal,GreaterThanOrEqual);

            default:
                // UNREACHABLE
                return InterpreterResult::error(InterpreterError::ReachedUnreachableError);
        }
        #undef DO_OP_LEFT
        #undef LEFT_CASE
        #undef DO_OP_RIGHT
        #undef RIGHT_CASE
        #undef DO_CASTED_OP

        SCC_SUPPRESS_WARNING_POP
        return InterpreterResult::ok(1);
    }
}
