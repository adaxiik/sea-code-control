#include "parser.hpp"
#include <iostream>
#include <algorithm>
#include <cstring>
#include <set>
namespace scc
{
    Parser::Parser() : m_parser(std::unique_ptr<TSParser, decltype(&ts_parser_delete)>(ts_parser_new(), ts_parser_delete))
                     , m_language(tree_sitter_c())

    {
        ts_parser_set_language(m_parser.get(), m_language);
    }

    std::string Parser::get_symbol_name(TSSymbol symbol) const
    {
        return ts_language_symbol_name(m_language, symbol);
    }


    ParserResult Parser::parse(const std::string &code)
    {
        return ParserResult(code, std::unique_ptr<TSTree, decltype(&ts_tree_delete)>(
            ts_parser_parse_string(
                m_parser.get(), 
                nullptr, 
                code.c_str(),
                code.size())
            , ts_tree_delete));
    }


    // =======================
    ParserResult::ParserResult(const std::string &code, TSTreePtr tree)
        : m_code(code)
        , m_tree(std::move(tree))
    {
    }


    TSNode ParserResult::get_root_node() const
    {
        return ts_tree_root_node(m_tree.get());
    }

    bool ParserResult::has_error() const
    {
        return ts_node_has_error(get_root_node());
    }

    const std::string& ParserResult::get_code() const
    {
        return m_code;
    }

    std::string ParserResult::get_node_value(TSNode node) const
    {
        return m_code.substr(ts_node_start_byte(node), ts_node_end_byte(node) - ts_node_start_byte(node));
    }

}