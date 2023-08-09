#include "interpreter.hpp"

#include <iostream>
#include "debug.hpp"
#include "cpp_compiler.hpp"
#include "operation_result.hpp"




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
        if (binded.is_error())
            return InterpreterError::BindError;

        if (binded.get_value()->bound_node_kind() != binding::BoundNodeKind::BlockStatement)
            return InterpreterError::BindError;
        
        TRACE();
        return interpret(ikwid_rc<binding::BoundBlockStatement>(*binded.get_value()));
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

    InterpreterResult Interpreter::interpret(const binding::BoundVariableDeclarationStatement &variable_declaration)
    {
        TRACE();
        // TODOOOOOOOOOOOOOOOOOOOO:
        return InterpreterError::RuntimeError;
    }

    InterpreterResult Interpreter::interpret(const binding::BoundStatement &statement)
    {
        TRACE();
        static_assert(binding::STATEMENT_COUNT == 3, "Update this code");
        switch (statement.bound_node_kind())
        {
        case binding::BoundNodeKind::ExpressionStatement:
            return eval(*ikwid_rc<binding::BoundExpressionStatement>(statement).expression);
        case binding::BoundNodeKind::BlockStatement:
            return interpret(ikwid_rc<binding::BoundBlockStatement>(statement));
        case binding::BoundNodeKind::VariableDeclarationStatement:
            return interpret(ikwid_rc<binding::BoundVariableDeclarationStatement>(statement));
        default:
            // UNREACHABLE
            return InterpreterError::BindError;
        }
    }

    InterpreterResult Interpreter::eval(const binding::BoundExpression& expression)
    {
        TRACE();
        static_assert(binding::EXPRESSION_COUNT == 4, "Update this code");
        switch (expression.bound_node_kind())
        {
        case binding::BoundNodeKind::BinaryExpression:
            return eval(ikwid_rc<binding::BoundBinaryExpression>(expression));
        case binding::BoundNodeKind::LiteralExpression:
        {
            // return interpret(ikwid_rc<binding::BoundLiteralExpression>(expression));
            decltype(auto) literal = ikwid_rc<binding::BoundLiteralExpression>(expression);
            return InterpreterResult::ok(InterpreterResultValue(literal.value, literal.type));
        }
        case binding::BoundNodeKind::CastExpression:
            return eval(ikwid_rc<binding::BoundCastExpression>(expression));
        case binding::BoundNodeKind::ParenthesizedExpression:
            return eval(ikwid_rc<binding::BoundParenthesizedExpression>(expression));
        default:
            // UNREACHABLE
            return InterpreterResult::error(InterpreterError::ReachedUnreachableError);
        }
    }
    
    

    

    InterpreterResult Interpreter::eval(const binding::BoundParenthesizedExpression &parenthesized_expression)
    {
        TRACE();
        for (size_t i = 0; i < parenthesized_expression.expressions.size(); i++)
        {
            auto result{eval(*parenthesized_expression.expressions[i])};
            if (result.is_error())
                return result;
            
            if (i == parenthesized_expression.expressions.size() - 1)
                return result;
        }

        // UNREACHABLE
        return InterpreterResult::error(InterpreterError::ReachedUnreachableError);
    }
}
