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

#include "binding/bound_binary_expression.hpp"
#include "binding/bound_literal_expression.hpp"
#include "binding/bound_cast_expression.hpp"
#include "binding/bound_parenthesized_expression.hpp"
#include "binding/bound_identifier_expression.hpp"
#include "binding/bound_assignment_expression.hpp"
#include "binding/bound_call_expression.hpp"

namespace scc
{
    class Lowerer
    {   
        using BoundNodeType = const binding::BoundNode *;
        using InstructionType = lowering::Instruction;
        using BoundNodeOrInstruction = std::variant<BoundNodeType, InstructionType>;

        std::stack<BoundNodeOrInstruction> m_to_lower;

        bool should_drop_after_statement(const binding::BoundExpressionStatement& expression_statement);

        static_assert(static_cast<int>(binding::BoundNodeKind::COUNT) == 17);

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

        void lower(const binding::BoundBinaryExpression &binary_expression);
        void lower(const binding::BoundLiteralExpression &literal_expression);
        void lower(const binding::BoundCastExpression &cast_expression);
        void lower(const binding::BoundParenthesizedExpression &parenthesized_expression);
        void lower(const binding::BoundIdentifierExpression &identifier_expression);
        void lower(const binding::BoundAssignmentExpression &assignment_expression);
        void lower(const binding::BoundCallExpression &call_expression);

    public:
        std::vector<lowering::Instruction> lower(const binding::BoundNode *root);
    };
}