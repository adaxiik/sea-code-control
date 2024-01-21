#pragma once
#include <vector>
#include <memory>
#include <stack>

#include "lowering/instruction.hpp"
#include "binding/bound_node.hpp"
#include "binding/bound_expression_statement.hpp"
#include "binding/bound_block_statement.hpp"
#include "binding/bound_variable_declaration_statement.hpp"
#include "binding/bound_if_statement.hpp"
#include "binding/bound_while_statement.hpp"
#include "binding/bound_do_statement.hpp"
#include "binding/bound_break_statement.hpp"
#include "binding/bound_continue_statement.hpp"
#include "binding/bound_function_statement.hpp"
#include "binding/bound_return_statement.hpp"
#include "binding/bound_for_statement.hpp"

#include "binding/bound_binary_expression.hpp"
#include "binding/bound_literal_expression.hpp"
#include "binding/bound_cast_expression.hpp"
#include "binding/bound_parenthesized_expression.hpp"
#include "binding/bound_identifier_expression.hpp"
#include "binding/bound_assignment_expression.hpp"
#include "binding/bound_call_expression.hpp"
#include "binding/bound_reference_expression.hpp"
#include "binding/bound_dereference_expression.hpp"
#include "binding/bound_pointer_assignment_expression.hpp"

#include "location.hpp"

namespace scc
{
    class Lowerer
    {   
        using Label = size_t;
        Label m_current_label;
        Label create_label();

        struct LoopLabels
        {
            Label continue_label;
            Label break_label;
            LoopLabels(Label continue_label, Label break_label): continue_label(continue_label), break_label(break_label) {}
        };

        std::stack<LoopLabels> m_loop_labels;

        using PushLabels = LoopLabels;
        struct PopLabels{};

        // Dirty lowering signals
        using BoundNodeType = const binding::BoundNode *;
        using InstructionType = std::pair<lowering::Instruction, std::optional<Location>>;
        using ToLower = std::variant<BoundNodeType, InstructionType, PushLabels, PopLabels>;

        std::stack<ToLower> m_to_lower;

        bool should_drop_after_statement(const binding::BoundStatement* statement);

        static_assert(static_cast<int>(binding::BoundNodeKind::COUNT) == 21);

        void lower(const binding::BoundExpressionStatement &expression_statement);
        void lower(const binding::BoundBlockStatement &block_statement);
        void lower(const binding::BoundVariableDeclarationStatement &variable_declaration_statement);
        void lower(const binding::BoundVariableValueDeclarationStatement &variable_value_declaration_statement);
        void lower(const binding::BoundIfStatement &if_statement);
        void lower(const binding::BoundWhileStatement &while_statement);
        void lower(const binding::BoundDoStatement &do_statement);
        void lower(const binding::BoundBreakStatement &break_statement);
        void lower(const binding::BoundContinueStatement &continue_statement);
        void lower(const binding::BoundFunctionStatement &function_statement);
        void lower(const binding::BoundReturnStatement &return_statement);
        void lower(const binding::BoundForStatement &for_statement);

        void lower(const binding::BoundBinaryExpression &binary_expression);
        void lower(const binding::BoundLiteralExpression &literal_expression);
        void lower(const binding::BoundCastExpression &cast_expression);
        void lower(const binding::BoundParenthesizedExpression &parenthesized_expression);
        void lower(const binding::BoundIdentifierExpression &identifier_expression);
        void lower(const binding::BoundAssignmentExpression &assignment_expression);
        void lower(const binding::BoundCallExpression &call_expression);
        void lower_user_fn_call(const binding::BoundCallExpression &call_expression);
        void lower_inbuild_fn_call(const binding::BoundCallExpression &call_expression);
        void lower(const binding::BoundReferenceExpression &pointer_expression);
        void lower(const binding::BoundVariablePointerDeclarationStatement &variable_pointer_declaration_statement);
        void lower(const binding::BoundVariableStaticArrayDeclarationStatement &variable_pointer_declaration_statement);
        void lower(const binding::BoundDereferenceExpression &pointer_expression);
        void lower(const binding::BoundPointerAssignmentExpression &pointer_assignment_expression);

    public:
        std::vector<std::pair<lowering::Instruction, std::optional<Location>>> lower(const binding::BoundBlockStatement* root);
        Lowerer(): m_current_label(0) {}
    };
}