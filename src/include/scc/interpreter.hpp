#pragma once
#include "parser.hpp"
#include "binding/binder.hpp"
#include "interpreter_result.hpp"

namespace scc
{
    class Interpreter
    {
    public:
        Interpreter() = default;
        ~Interpreter() = default;
        ParserResult parse(const std::string &code);
        InterpreterResult interpret(const std::string &code);
        InterpreterResult interpret(const ParserResult &parse_result);

    private:
        template <typename T, typename U>
        const T& ikwid_rc (const U& u) // I know what I'm doing reference cast (used for downcasting)
        {
            static_assert(std::is_base_of_v<U,T>, "U must be a base of T");
            return *static_cast<const T*>(&u);
        }

        Parser m_parser;

        InterpreterResult interpret(const binding::BoundBlockStatement &block_statement);
        InterpreterResult interpret(const binding::BoundStatement &statement);

        InterpreterResult eval(const binding::BoundExpression &expression);
        InterpreterResult eval(const binding::BoundBinaryExpression &binary_expression);
        InterpreterResult eval(const binding::BoundCastExpression &cast_expression);

    };

}