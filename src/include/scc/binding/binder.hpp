#pragma once
#include <optional>
#include "treenode.hpp"
#include "binding/bound_node.hpp"
#include "binding/bound_block_statement.hpp"
#include "binding/bound_expression.hpp"
#include "binding/bound_expression_statement.hpp"
#include "binding/bound_binary_expression.hpp"
#include "binding/bound_literal_expression.hpp"
#include "binding/bound_cast_expression.hpp"
#include "binding/bound_parenthesized_expression.hpp"
#include "binding/bound_variable_declaration_statement.hpp"
namespace scc
{

    class Binder
    {
        
        static_assert(static_cast<int>(binding::BoundNodeKind::COUNT) == 7, "Update this code");
        static_assert(std::is_base_of_v<binding::BoundStatement, binding::BoundExpressionStatement>, "ExpressionStatement must be derived from BoundStatement");
        static_assert(std::is_base_of_v<binding::BoundStatement, binding::BoundBlockStatement>, "BlockStatement must be derived from BoundStatement");
        static_assert(std::is_base_of_v<binding::BoundExpression, binding::BoundBinaryExpression>, "BinaryExpression must be derived from BoundExpression");
        static_assert(std::is_base_of_v<binding::BoundExpression, binding::BoundLiteralExpression>, "LiteralExpression must be derived from BoundExpression");
        static_assert(std::is_base_of_v<binding::BoundExpression, binding::BoundCastExpression>, "CastExpression must be derived from BoundExpression");
        static_assert(std::is_base_of_v<binding::BoundExpression, binding::BoundParenthesizedExpression>, "ParenthesizedExpression must be derived from BoundExpression");
        static_assert(std::is_base_of_v<binding::BoundStatement, binding::BoundVariableDeclarationStatement>, "VariableDeclaration must be derived from BoundStatement");
       
        Binder() = delete;
        ~Binder() = delete;

        static std::unique_ptr<binding::BoundBlockStatement> bind_block_statement(const TreeNode &node);
        static std::unique_ptr<binding::BoundNode> bind_impl(const TreeNode &node);
        static std::unique_ptr<binding::BoundExpressionStatement> bind_expression_statement(const TreeNode &node);
        static std::unique_ptr<binding::BoundExpression> bind_expression(const TreeNode &node);
        static std::unique_ptr<binding::BoundBinaryExpression> bind_binary_expression(const TreeNode &node);
        static std::unique_ptr<binding::BoundCastExpression> bind_cast_expression(const TreeNode &node);
        static std::unique_ptr<binding::BoundParenthesizedExpression> bind_parenthesized_expression(const TreeNode &node);
        static std::unique_ptr<binding::BoundLiteralExpression> bind_literal_expression(const TreeNode &node);
        static std::unique_ptr<binding::BoundLiteralExpression> bind_number_literal(const TreeNode &node);
        static std::unique_ptr<binding::BoundVariableDeclarationStatement> bind_variable_declaration(const TreeNode &node);
    public:
        static std::unique_ptr<binding::BoundNode> bind(const TreeNode &node);
    };  
}