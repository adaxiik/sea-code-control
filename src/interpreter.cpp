#include "interpreter.hpp"

#include <iostream>
#include "debug.hpp"

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
            return InterpreterResult::ParseError;

        auto binded {Binder::bind(parse_result.root_node())};
        if (!binded)
            return InterpreterResult::BindError;

        if (binded->bound_node_kind() != binding::BoundNodeKind::BlockStatement)
            return InterpreterResult::BindError;
        
        
        return interpret(*static_cast<binding::BoundBlockStatement*>(binded.get()));
    }

    InterpreterResult Interpreter::interpret(const binding::BoundBlockStatement &block_statement)
    {
        debug::bound_ast_as_text_tree(std::cout, block_statement);

        return InterpreterResult::Ok;
    }

}
