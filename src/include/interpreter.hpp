#pragma once
#include "parser.hpp"
#include "binding/binder.hpp"


namespace scc
{

    enum class InterpreterResult
    {
        Ok,
        ParseError,
        BindError,
    };

    class Interpreter
    {
    public:
        Interpreter() = default;
        ~Interpreter() = default;
        ParserResult parse(const std::string &code);
        InterpreterResult interpret(const std::string &code);
        InterpreterResult interpret(const ParserResult &parse_result);

    private:
        Parser m_parser;

        InterpreterResult interpret(const binding::BoundBlockStatement &block_statement);

    };

}