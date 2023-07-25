#pragma once
#include "treenode.hpp"
#include "binding/bound_node.hpp"
#include "binding/bound_block_statement.hpp"
#include "binding/bound_expression.hpp"
#include "binding/bound_expression_statement.hpp"
#include "binding/bound_binary_expression.hpp"
#include "binding/bound_binary_operator.hpp"
#include "binding/bound_literal_expression.hpp"
#include <optional>
namespace scc
{

    class Binder
    {
        Binder() = delete;
        ~Binder() = delete;

        static std::unique_ptr<binding::BoundBlockStatement> bind_block_statement(const TreeNode &node);
        static std::unique_ptr<binding::BoundNode> bind_impl(const TreeNode &node);
        static std::unique_ptr<binding::BoundExpressionStatement> bind_expression_statement(const TreeNode &node);
        static std::unique_ptr<binding::BoundExpression> bind_expression(const TreeNode &node);
        static std::unique_ptr<binding::BoundBinaryExpression> bind_binary_expression(const TreeNode &node);
        // static std::unique_ptr<binding::BoundBinaryOperator> bind_binary_operator(const TreeNode &node);
        static std::unique_ptr<binding::BoundLiteralExpression> bind_literal_expression(const TreeNode &node);
        static std::unique_ptr<binding::BoundLiteralExpression> bind_number_literal(const TreeNode &node);
    public:
        static std::unique_ptr<binding::BoundNode> bind(const TreeNode &node);
    };  
}