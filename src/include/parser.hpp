#pragma once

#include <tree_sitter/api.h>
#include <tree_sitter/parser.h>
#include <tree_sitter-c/parser-c.h>

#include <string>
#include <memory>

namespace scc
{
    class Parser
    {
    private:
        std::unique_ptr<TSParser, decltype(&ts_parser_delete)> m_parser;
        std::unique_ptr<TSTree, decltype(&ts_tree_delete)> m_tree;
        TSNode m_root_node;
        std::string m_code;
        
    public:
        Parser(const std::string &code);
        ~Parser() = default;

        TSNode& get_root_node();
        bool has_error() const;

    };
} 
