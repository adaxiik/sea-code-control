#include "treenode.hpp"
#include "parser.hpp"
namespace scc
{
    TreeNode::TreeNode(TSNode node, const ParserResult& parser_result)
        : m_node(node)
        , m_parser_result(parser_result)
    {
    }

    std::string TreeNode::value() const
    {
        return m_parser_result
                        .code()
                        .substr(ts_node_start_byte(m_node), ts_node_end_byte(m_node) - ts_node_start_byte(m_node));
    }

    bool TreeNode::has_error() const
    {
        return ts_node_has_error(m_node);
    }

    TSSymbol TreeNode::symbol() const
    {
        return ts_node_symbol(m_node);
    }

    std::string TreeNode::symbol_name() const
    {
        return m_parser_result.parser().get_symbol_name(symbol());
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


}