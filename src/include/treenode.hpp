#pragma once
#include <tree_sitter/api.h>
#include <tree_sitter/parser.h>
#include <tree_sitter-c/parser-c.h>

#include <string>

namespace scc
{
    class ParserResult;
    class TreeNode
    {
        TSNode m_node;
        const ParserResult &m_parser_result;

    public:
        TreeNode(TSNode node, const ParserResult &parser_result);
        ~TreeNode() = default;

        std::string value() const;
        bool has_error() const;
        TSSymbol symbol() const;
        std::string symbol_name() const;

        uint32_t child_count() const;
        uint32_t named_child_count() const;

        TreeNode child(uint32_t index) const;
        TreeNode named_child(uint32_t index) const;

        // class NamedChildGenerator
        // {
        //     class NamedChildIterator
        //     {
        //         const TreeNode &m_node;
        //         size_t m_index;

        //     public:
        //         NamedChildIterator &operator++()
        //         {
        //             m_index++;
        //             return *this;
        //         }
        //         bool operator==(const NamedChildIterator &other) const
        //         {
        //             return this->m_index == other.m_index;
        //         }
        //         bool operator!=(const NamedChildIterator &other) const
        //         {
        //             return !(*this == other);
        //         }
        //         TreeNode operator*() const
        //         {
        //             return m_node.named_child(m_index);
        //         }

        //         NamedChildIterator(const TreeNode &node, size_t index)
        //             : m_node(node), m_index(index){};
        //     };
        //     const TreeNode &m_node;

        // public:
        //     NamedChildIterator begin() const
        //     {
        //         return NamedChildIterator(m_node, 0);
        //     };

        //     NamedChildIterator end() const
        //     {
        //         return NamedChildIterator(m_node, m_node.named_child_count());
        //     };

        // public:
        //     NamedChildGenerator(const TreeNode &node)
        //         : m_node(node){};
        // };

        // NamedChildGenerator named_children_iterator() const
        // {
        //     return NamedChildGenerator(*this);
        // };
    };
}