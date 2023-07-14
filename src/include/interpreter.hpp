#pragma once
#include "parser.hpp"

namespace scc
{

    enum class InterpreterResult
    {
        Ok,
        ParseError,
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
        InterpreterResult evaluate(const ParserResult &parse_result);
    };

}