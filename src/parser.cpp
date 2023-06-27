#include "parser.hpp"

namespace scc
{
    Parser::Parser(const std::string &code) : m_parser(std::unique_ptr<TSParser, decltype(&ts_parser_delete)>(ts_parser_new(), ts_parser_delete))
                                            , m_tree(nullptr, ts_tree_delete)
                                            , m_code(code)

    {
        ts_parser_set_language(m_parser.get(), tree_sitter_c());
        
        m_tree = std::unique_ptr<TSTree, decltype(&ts_tree_delete)>(
            ts_parser_parse_string(
                m_parser.get(), 
                nullptr, 
                m_code.c_str(),
                m_code.size())
            , ts_tree_delete);

        m_root_node = ts_tree_root_node(m_tree.get());
    }

    TSNode Parser::get_root_node() const
    {
        return m_root_node;
    }

    bool Parser::has_error() const
    {
        return ts_node_has_error(m_root_node);
    }

    const std::string& Parser::get_code() const
    {
        return m_code;
    }

}