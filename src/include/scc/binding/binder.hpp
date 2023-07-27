#pragma once
#include "treenode.hpp"
#include "binding/bound_node.hpp"
#include "binding/bound_block_statement.hpp"
#include "binding/bound_expression.hpp"
#include "binding/bound_expression_statement.hpp"
#include "binding/bound_binary_expression.hpp"
#include "binding/bound_literal_expression.hpp"
#include <optional>
namespace scc
{

    class Binder
    {
        
        static_assert(static_cast<int>(binding::BoundNodeKind::COUNT) == 4, "Update this code");
        static_assert(std::is_base_of_v<binding::BoundStatement, binding::BoundExpressionStatement>, "ExpressionStatement must be derived from BoundStatement");
        static_assert(std::is_base_of_v<binding::BoundStatement, binding::BoundBlockStatement>, "BlockStatement must be derived from BoundStatement");
        static_assert(std::is_base_of_v<binding::BoundExpression, binding::BoundBinaryExpression>, "BinaryExpression must be derived from BoundExpression");
        static_assert(std::is_base_of_v<binding::BoundExpression, binding::BoundLiteralExpression>, "LiteralExpression must be derived from BoundExpression");


        Binder() = delete;
        ~Binder() = delete;

        static std::unique_ptr<binding::BoundBlockStatement> bind_block_statement(const TreeNode &node);
        static std::unique_ptr<binding::BoundNode> bind_impl(const TreeNode &node);
        static std::unique_ptr<binding::BoundExpressionStatement> bind_expression_statement(const TreeNode &node);
        static std::unique_ptr<binding::BoundExpression> bind_expression(const TreeNode &node);
        static std::unique_ptr<binding::BoundBinaryExpression> bind_binary_expression(const TreeNode &node);
        static std::unique_ptr<binding::BoundLiteralExpression> bind_literal_expression(const TreeNode &node);
        static std::unique_ptr<binding::BoundLiteralExpression> bind_number_literal(const TreeNode &node);
    public:
        static std::unique_ptr<binding::BoundNode> bind(const TreeNode &node);
    };  
}