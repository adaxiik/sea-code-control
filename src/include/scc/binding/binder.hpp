#pragma once
#include <optional>
#include "treenode.hpp"
#include "binding/binder_scope.hpp"

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
#include "binding/bound_identifier_expression.hpp"
#include "binding/bound_assignment_expression.hpp"
#include "binding/bound_if_statement.hpp"
#include "binding/bound_while_statement.hpp"
#include "binding/bound_do_statement.hpp"
#include "binding/bound_break_statement.hpp"
#include "binding/bound_continue_statement.hpp"
#include "binding/bound_function.hpp"
namespace scc
{

    class Binder
    {
        
        static_assert(static_cast<int>(binding::BoundNodeKind::COUNT) == 14, "Update this code");
        static_assert(std::is_base_of_v<binding::BoundStatement, binding::BoundExpressionStatement>, "ExpressionStatement must be derived from BoundStatement");
        static_assert(std::is_base_of_v<binding::BoundStatement, binding::BoundBlockStatement>, "BlockStatement must be derived from BoundStatement");
        static_assert(std::is_base_of_v<binding::BoundExpression, binding::BoundBinaryExpression>, "BinaryExpression must be derived from BoundExpression");
        static_assert(std::is_base_of_v<binding::BoundExpression, binding::BoundLiteralExpression>, "LiteralExpression must be derived from BoundExpression");
        static_assert(std::is_base_of_v<binding::BoundExpression, binding::BoundCastExpression>, "CastExpression must be derived from BoundExpression");
        static_assert(std::is_base_of_v<binding::BoundExpression, binding::BoundParenthesizedExpression>, "ParenthesizedExpression must be derived from BoundExpression");
        static_assert(std::is_base_of_v<binding::BoundStatement, binding::BoundVariableDeclarationStatement>, "VariableDeclaration must be derived from BoundStatement");
        static_assert(std::is_base_of_v<binding::BoundExpression, binding::BoundIdentifierExpression>, "IdentifierExpression must be derived from BoundExpression");
        static_assert(std::is_base_of_v<binding::BoundExpression, binding::BoundAssignmentExpression>, "AssignmentExpression must be derived from BoundExpression");
        static_assert(std::is_base_of_v<binding::BoundStatement, binding::BoundIfStatement>, "IfStatement must be derived from BoundStatement");
        static_assert(std::is_base_of_v<binding::BoundStatement, binding::BoundWhileStatement>, "WhileStatement must be derived from BoundStatement");
        static_assert(std::is_base_of_v<binding::BoundStatement, binding::BoundDoStatement>, "DoStatement must be derived from BoundStatement");
        static_assert(std::is_base_of_v<binding::BoundStatement, binding::BoundBreakStatement>, "BreakStatement must be derived from BoundStatement");
        static_assert(std::is_base_of_v<binding::BoundStatement, binding::BoundContinueStatement>, "ContinueStatement must be derived from BoundStatement");
        static_assert(std::is_base_of_v<binding::BoundNode, binding::BoundFunction>, "Function must be derived from BoundStatement");

        Binder(const Binder &) = delete;
        Binder(Binder &&) = delete;
        Binder &operator=(const Binder &) = delete;
        Binder &operator=(Binder &&) = delete;

        binding::BinderScopeStack m_scope_stack;
        
        binding::BinderResult<binding::BoundBlockStatement> bind_block_statement(const TreeNode &node);
        binding::BinderResult<binding::BoundNode> bind_impl(const TreeNode &node);
        binding::BinderResult<binding::BoundExpressionStatement> bind_expression_statement(const TreeNode &node);
        binding::BinderResult<binding::BoundExpression> bind_expression(const TreeNode &node);
        binding::BinderResult<binding::BoundBinaryExpression> bind_binary_expression(const TreeNode &node);
        binding::BinderResult<binding::BoundCastExpression> bind_cast_expression(const TreeNode &node);
        binding::BinderResult<binding::BoundParenthesizedExpression> bind_parenthesized_expression(const TreeNode &node);
        binding::BinderResult<binding::BoundLiteralExpression> bind_literal_expression(const TreeNode &node);
        binding::BinderResult<binding::BoundLiteralExpression> bind_number_literal(const TreeNode &node);
        binding::BinderResult<binding::BoundVariableDeclarationStatement> bind_variable_declaration(const TreeNode &node);
        binding::BinderResult<binding::BoundIdentifierExpression> bind_identifier_expression(const TreeNode &node);
        binding::BinderResult<binding::BoundAssignmentExpression> bind_assignment_expression(const TreeNode &node);
        binding::BinderResult<binding::BoundIfStatement> bind_if_statement(const TreeNode &node);
        binding::BinderResult<binding::BoundWhileStatement> bind_while_statement(const TreeNode &node);
        binding::BinderResult<binding::BoundDoStatement> bind_do_statement(const TreeNode &node);
        binding::BinderResult<binding::BoundBreakStatement> bind_break_statement(const TreeNode &node);
        binding::BinderResult<binding::BoundContinueStatement> bind_continue_statement(const TreeNode &node);
        binding::BinderResult<binding::BoundNode> bind_declaration(const TreeNode &node);
        binding::BinderResult<binding::BoundFunction> bind_function(const TreeNode &node);
    public:
        Binder() { m_scope_stack.push();}
        ~Binder() = default;
        binding::BinderResult<binding::BoundNode> bind(const TreeNode &node);
    };  
}