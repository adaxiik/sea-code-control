#include "treenode.hpp"
#include "parser.hpp"

#include <queue>

namespace scc
{
    std::string TreeNode::value() const
    {
        return m_parser_result
                        ->code()
                        .substr(ts_node_start_byte(m_node), ts_node_end_byte(m_node) - ts_node_start_byte(m_node));
    }

    bool TreeNode::has_error() const
    {
        return ts_node_has_error(m_node);
    }

    bool TreeNode::has_parent() const
    {
        return not ts_node_is_null(ts_node_parent(m_node));
    }

    TSSymbol TreeNode::symbol() const
    {
        return ts_node_symbol(m_node);
    }

    std::string TreeNode::symbol_name() const
    {
        return m_parser_result->parser().get_symbol_name(symbol());
    }

    uint32_t TreeNode::child_count() const
    {
        return ts_node_child_count(m_node);
    }

    uint32_t TreeNode::named_child_count() const
    {
        return ts_node_named_child_count(m_node);
    }

    TreeNode TreeNode::child(uint32_t index) const
    {
        return TreeNode(ts_node_child(m_node, index), m_parser_result);
    }

    TreeNode TreeNode::named_child(uint32_t index) const
    {
        return TreeNode(ts_node_named_child(m_node, index), m_parser_result);
    }

    TreeNode TreeNode::first_child() const
    {
        return TreeNode(ts_node_child(m_node, 0), m_parser_result);
    }

    TreeNode TreeNode::first_named_child() const
    {
        return TreeNode(ts_node_named_child(m_node, 0), m_parser_result);
    }

    TreeNode TreeNode::last_child() const
    {
        return TreeNode(ts_node_child(m_node, ts_node_child_count(m_node) - 1), m_parser_result);
    }

    TreeNode TreeNode::last_named_child() const
    {
        return TreeNode(ts_node_named_child(m_node, ts_node_named_child_count(m_node) - 1), m_parser_result);
    }

    Location TreeNode::location() const
    {
        TSPoint start_point = ts_node_start_point(m_node);
        return Location{start_point.row, start_point.column};
    }

    std::optional<Location> TreeNode::maybe_location() const
    {
        if (m_parser_result->want_to_remove_location())
            return std::nullopt;

        TSPoint start_point = ts_node_start_point(m_node);
        return Location{start_point.row, start_point.column};
    }

    std::optional<TreeNode> TreeNode::named_child_with_symbol_BFS(TSSymbol symbol, SearchDepth max_depth) const
    {
        // BFS, depth 0 is infinite
        std::queue<TreeNode> queue;
        queue.push(*this);
        uint32_t depth = 0;

        while (!queue.empty())
        {
            TreeNode node = queue.front();
            queue.pop();

            if (node.symbol() == symbol)
                return node;

            if (max_depth != SearchDepth::infinite && depth >= max_depth)
                continue;

            for (uint32_t i = 0; i < node.named_child_count(); i++)
                queue.push(node.named_child(i));

            depth++;
        }

        return std::nullopt;
    }

    size_t TreeNode::count_named_children_with_symbol(TSSymbol symbol, SearchDepth max_depth) const
    {
        size_t count = 0;
        std::queue<TreeNode> queue;
        queue.push(*this);
        uint32_t depth = 0;

        while (!queue.empty())
        {
            TreeNode node = queue.front();
            queue.pop();

            if (node.symbol() == symbol)
                count++;

            if (max_depth != SearchDepth::infinite && depth >= max_depth)
                continue;

            for (uint32_t i = 0; i < node.named_child_count(); i++)
                queue.push(node.named_child(i));

            depth++;
        }

        return count;
    }

    bool TreeNode::has_named_child_with_symbol(TSSymbol symbol, SearchDepth max_depth) const
    {
        return named_child_with_symbol_BFS(symbol, max_depth).has_value();
    }

}