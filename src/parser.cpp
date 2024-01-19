#include "parser.hpp"
#include <iostream>
#include <algorithm>
#include <cstring>
#include <set>
namespace scc
{
    Parser::Parser() 
    : m_parser(std::unique_ptr<TSParser, decltype(&ts_parser_delete)>(ts_parser_new(), ts_parser_delete))
    , m_language(tree_sitter_c())
    {
        ts_parser_set_language(m_parser.get(), m_language);
    }

    std::string Parser::get_symbol_name(TSSymbol symbol) const
    {
        return ts_language_symbol_name(m_language, symbol);
    }


    ParserResult Parser::parse(const std::string &code, bool want_to_remove_location) const
    {
        return ParserResult(code, std::unique_ptr<TSTree, decltype(&ts_tree_delete)>(
            ts_parser_parse_string(
                m_parser.get(), 
                nullptr, 
                code.c_str(),
                code.size())
            , ts_tree_delete)
            , *this
            , want_to_remove_location);
    }


    // =======================
    ParserResult::ParserResult(const std::string &code
                             , TSTreePtr tree
                             , const Parser& parser
                             , bool want_to_remove_location)
        : m_code(code)
        , m_tree(std::move(tree))
        , m_parser(parser)
        , m_want_to_remove_location(want_to_remove_location)
    {
    }

    ParserResult::ParserResult(const ParserResult& other)
        : m_code(other.m_code)
        , m_tree(std::unique_ptr<TSTree, decltype(&ts_tree_delete)>(
            ts_tree_copy(other.m_tree.get())
            , ts_tree_delete))
        , m_parser(other.m_parser)
        , m_want_to_remove_location(other.m_want_to_remove_location)
    {
    }

    const Parser& ParserResult::parser() const
    {
        return m_parser;
    }

    TreeNode ParserResult::root_node() const
    {

        return TreeNode(ts_tree_root_node(m_tree.get()),
                        *this);
        // return ts_tree_root_node(m_tree.get());
    }

    bool ParserResult::has_error() const
    {
        return root_node().has_error();
    }

    const std::string& ParserResult::code() const
    {
        return m_code;
    }

    bool ParserResult::want_to_remove_location() const
    {
        return m_want_to_remove_location;
    }
    
    // std::string ParserResult::get_node_value(TSNode node) const
    // {
    //     return m_code.substr(ts_node_start_byte(node), ts_node_end_byte(node) - ts_node_start_byte(node));
    // }

}