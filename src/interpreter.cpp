#include "interpreter.hpp"

#include <iostream>

namespace scc
{
    ParserResult Interpreter::parse(const std::string &code)
    {
        return m_parser.parse(code);
    }

    InterpreterResult Interpreter::interpret(const std::string &code)
    {
        auto parse_result = m_parser.parse(code);
        if (parse_result.has_error())
            return InterpreterResult::ParseError;

        return evaluate(parse_result);
    }

    InterpreterResult Interpreter::interpret(const ParserResult &parse_result)
    {
        return evaluate(parse_result);
    }


    InterpreterResult Interpreter::evaluate(const ParserResult &parse_result)
    {
        auto root_node = parse_result.root_node();
        auto root_node_type = root_node.symbol_name();
        std::cout << "Root node type: " << root_node_type << std::endl;

        return InterpreterResult::Ok;
    }
}
