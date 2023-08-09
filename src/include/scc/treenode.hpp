#pragma once
#include <tree_sitter/api.h>
#include <tree_sitter/parser.h>
#include <tree_sitter-c/parser-c.h>

#include <string>
#include <algorithm>
#include "location.hpp"

namespace scc
{
    class ParserResult;
    class TreeNode
    {
        TSNode m_node;
        const ParserResult *m_parser_result;

    public:
        TreeNode(TSNode node, const ParserResult &parser_result)
            : m_node(node)
            , m_parser_result(&parser_result){}

        TreeNode(TSNode node, const ParserResult *parser_result) 
            : m_node(node)
            , m_parser_result(parser_result){}
        
        ~TreeNode() = default;

        TreeNode(const TreeNode & node) : m_node(node.m_node), m_parser_result(node.m_parser_result) {}
        TreeNode & operator=(const TreeNode & node)
        {
            if (this != &node)
            {
                m_node = node.m_node;
                m_parser_result = node.m_parser_result;
            }
            return *this;
        }

        std::string value() const;
        bool has_error() const;
        TSSymbol symbol() const;
        std::string symbol_name() const;

        uint32_t child_count() const;
        uint32_t named_child_count() const;

        TreeNode child(uint32_t index) const;
        TreeNode named_child(uint32_t index) const;

        TreeNode first_child() const;
        TreeNode first_named_child() const;
        TreeNode last_child() const;
        TreeNode last_named_child() const;
        Location location() const;
    };
}