#pragma once
#include <optional>
#include "treenode.hpp"
#include "binding/binder_result.hpp"
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

        static binding::BinderResult<binding::BoundBlockStatement> bind_block_statement(const TreeNode &node);
        static binding::BinderResult<binding::BoundNode> bind_impl(const TreeNode &node);
        static binding::BinderResult<binding::BoundExpressionStatement> bind_expression_statement(const TreeNode &node);
        static binding::BinderResult<binding::BoundExpression> bind_expression(const TreeNode &node);
        static binding::BinderResult<binding::BoundBinaryExpression> bind_binary_expression(const TreeNode &node);
        static binding::BinderResult<binding::BoundCastExpression> bind_cast_expression(const TreeNode &node);
        static binding::BinderResult<binding::BoundParenthesizedExpression> bind_parenthesized_expression(const TreeNode &node);
        static binding::BinderResult<binding::BoundLiteralExpression> bind_literal_expression(const TreeNode &node);
        static binding::BinderResult<binding::BoundLiteralExpression> bind_number_literal(const TreeNode &node);
        static binding::BinderResult<binding::BoundVariableDeclarationStatement> bind_variable_declaration(const TreeNode &node);
    public:
        static binding::BinderResult<binding::BoundNode> bind(const TreeNode &node);
    };  
}