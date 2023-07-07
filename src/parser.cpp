#include "parser.hpp"
#include <iostream>
#include <algorithm>
#include <cstring>
#include <set>
namespace scc
{
    Parser::Parser(const std::string &code) : m_parser(std::unique_ptr<TSParser, decltype(&ts_parser_delete)>(ts_parser_new(), ts_parser_delete))
                                            , m_tree(nullptr, ts_tree_delete)
                                            , m_code(code)
                                            , m_language(tree_sitter_c())

    {
        // size_t field_count = ts_language_field_count(language);
        // std::cout << "field count: " << field_count << std::endl;
        // for (size_t i = 1; i <= field_count; i++)
        // {
        //     const char* name = ts_language_field_name_for_id(language, i);
        //     std::cout << "field id: " << i << " name: " << name << std::endl;
        // }

        // size_t symbol_count = ts_language_symbol_count(language);
        // std::cout << "symbol count: " << symbol_count << std::endl;
        // std::set<std::string> names;
        // for (size_t i = 0; i < symbol_count; i++)
        // {
        //     const char* name = ts_language_symbol_name(language, i);
        //     TSSymbolType type = ts_language_symbol_type(language, i);
        //     if(type == TSSymbolTypeRegular)
        //     {
        //         // some symbol names are there multiple times, with different ids.. I dunno why
        //         auto s = ts_language_symbol_for_name(language, name, std::strlen(name), true);           
        //         std::string name_upper{name};
        //         std::transform(name_upper.begin(), name_upper.end(), name_upper.begin(), [](unsigned char c){ return std::toupper(c); });
        //         std::string result = "static constexpr auto " + name_upper + "_SYMBOL = " + std::to_string(s) + ";";
        //         names.insert(result);
        //     }
        // }

        // for(auto& name : names)
        //     std::cout << name << std::endl;

        ts_parser_set_language(m_parser.get(), m_language);
        
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

    std::string Parser::get_symbol_name(TSSymbol symbol) const
    {
        return ts_language_symbol_name(m_language, symbol);
    }

    std::string Parser::get_node_value(TSNode node) const
    {
        return m_code.substr(ts_node_start_byte(node), ts_node_end_byte(node) - ts_node_start_byte(node));
    }

}