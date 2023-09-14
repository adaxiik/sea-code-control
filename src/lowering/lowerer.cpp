#include "lowering/lowerer.hpp"
#include "cpp_compiler.hpp"

#include "lowering/push_literal_instruction.hpp"
#include "lowering/pop_scope_instruction.hpp"
#include "lowering/push_scope_instruction.hpp"
#include "lowering/binary_operation_instruction.hpp"
#include "lowering/cast_instruction.hpp"
#include "lowering/drop_instruction.hpp"

#include <iostream>
#include <iomanip>

#define SCC_NOT_IMPLEMENTED(x)                                           \
    do                                                                   \
    {                                                                    \
        std::cerr << "==============================" << std::endl;      \
        std::cerr << "Not implemented: " << std::quoted(x) << std::endl; \
        std::cerr << "At: " << __FILE__ << ":" << __LINE__ << std::endl; \
        std::cerr << "==============================" << std::endl;      \
        std::exit(1);                                                    \
    } while (0)

namespace scc
{
    
    void Lowerer::lower(const binding::BoundExpressionStatement &expression_statement)
    {
        m_to_lower.push(expression_statement.expression.get());
    }

    void Lowerer::lower(const binding::BoundBlockStatement &block_statement)
    {
        m_to_lower.push(std::make_unique<lowering::PopScopeInstruction>());

        for (auto &statement : block_statement.statements)
            m_to_lower.push(statement.get());

        m_to_lower.push(std::make_unique<lowering::PushScopeInstruction>());
    }

    void Lowerer::lower(const binding::BoundVariableDeclarationStatement &variable_declaration_statement)
    {
        SCC_NOT_IMPLEMENTED("BoundVariableDeclarationStatement");
    }

    void Lowerer::lower(const binding::BoundIfStatement &if_statement)
    {
        SCC_NOT_IMPLEMENTED("BoundIfStatement");
    }

    void Lowerer::lower(const binding::BoundWhileStatement &while_statement)
    {
        SCC_NOT_IMPLEMENTED("BoundWhileStatement");
    }

    void Lowerer::lower(const binding::BoundDoStatement &do_statement)
    {
        SCC_NOT_IMPLEMENTED("BoundDoStatement");
    }

    void Lowerer::lower(const binding::BoundBreakStatement &break_statement)
    {
        SCC_NOT_IMPLEMENTED("BoundBreakStatement");
    }

    void Lowerer::lower(const binding::BoundContinueStatement &continue_statement)
    {
        SCC_NOT_IMPLEMENTED("BoundContinueStatement");
    }

    void Lowerer::lower(const binding::BoundFunctionStatement &function_statement)
    {
        SCC_NOT_IMPLEMENTED("BoundFunctionStatement");
    }

    void Lowerer::lower(const binding::BoundReturnStatement &return_statement)
    {
        SCC_NOT_IMPLEMENTED("BoundReturnStatement");
    }

    void Lowerer::lower(const binding::BoundBinaryExpression &binary_expression)
    {
        m_to_lower.push(std::make_unique<lowering::BinaryOperationInstruction>(binary_expression.op_kind));
        m_to_lower.push(binary_expression.left.get());
        m_to_lower.push(binary_expression.right.get());
    }

    void Lowerer::lower(const binding::BoundLiteralExpression &literal_expression)
    {
        m_to_lower.push(std::make_unique<lowering::PushLiteralInstruction>(literal_expression));
    }

    void Lowerer::lower(const binding::BoundCastExpression &cast_expression)
    {
        m_to_lower.push(std::make_unique<lowering::CastInstruction>(cast_expression.type));
        m_to_lower.push(cast_expression.expression.get());
    }

    void Lowerer::lower(const binding::BoundParenthesizedExpression &parenthesized_expression)
    {
        m_to_lower.push(std::make_unique<lowering::DropInstruction>(parenthesized_expression.expressions.size() - 1));
        
        // c++20 ranges would be really nice here :(( 
        for (const auto& expression: parenthesized_expression.expressions)
            m_to_lower.push(expression.get());
    }

    void Lowerer::lower(const binding::BoundIdentifierExpression &identifier_expression)
    {
        SCC_NOT_IMPLEMENTED("BoundIdentifierExpression");
    }

    void Lowerer::lower(const binding::BoundAssignmentExpression &assignment_expression)
    {
        SCC_NOT_IMPLEMENTED("BoundAssignmentExpression");
    }

    void Lowerer::lower(const binding::BoundCallExpression &call_expression)
    {
        SCC_NOT_IMPLEMENTED("BoundCallExpression");
    }


    std::vector<std::unique_ptr<lowering::Instruction>> Lowerer::lower(const binding::BoundNode *root)
    {
        std::vector<std::unique_ptr<lowering::Instruction>> result;
        m_to_lower.push(root);

        while (!m_to_lower.empty())
        {
            auto& current_node_or_instruction = m_to_lower.top();
            if (std::holds_alternative<InstructionType>(current_node_or_instruction))
            {
                result.push_back(std::move(std::get<InstructionType>(current_node_or_instruction)));
                m_to_lower.pop();
                continue;
            }

            static_assert(static_cast<int>(binding::BoundNodeKind::COUNT) == 17);

            const auto current_node = std::get<BoundNodeType>(current_node_or_instruction);
            m_to_lower.pop();

            switch (current_node->bound_node_kind())
            {
            case binding::BoundNodeKind::ExpressionStatement:
                lower(*static_cast<const binding::BoundExpressionStatement *>(current_node));
                break;
            case binding::BoundNodeKind::BlockStatement:
                lower(*static_cast<const binding::BoundBlockStatement *>(current_node));
                break;
            case binding::BoundNodeKind::VariableDeclarationStatement:
                lower(*static_cast<const binding::BoundVariableDeclarationStatement *>(current_node));
                break;
            case binding::BoundNodeKind::IfStatement:
                lower(*static_cast<const binding::BoundIfStatement *>(current_node));
                break;
            case binding::BoundNodeKind::WhileStatement:
                lower(*static_cast<const binding::BoundWhileStatement *>(current_node));
                break;
            case binding::BoundNodeKind::DoStatement:
                lower(*static_cast<const binding::BoundDoStatement *>(current_node));
                break;
            case binding::BoundNodeKind::BreakStatement:
                lower(*static_cast<const binding::BoundBreakStatement *>(current_node));
                break;
            case binding::BoundNodeKind::ContinueStatement:
                lower(*static_cast<const binding::BoundContinueStatement *>(current_node));
                break;
            case binding::BoundNodeKind::FunctionStatement:
                lower(*static_cast<const binding::BoundFunctionStatement *>(current_node));
                break;
            case binding::BoundNodeKind::ReturnStatement:
                lower(*static_cast<const binding::BoundReturnStatement *>(current_node));
                break;
            case binding::BoundNodeKind::BinaryExpression:
                lower(*static_cast<const binding::BoundBinaryExpression *>(current_node));
                break;
            case binding::BoundNodeKind::LiteralExpression:
                lower(*static_cast<const binding::BoundLiteralExpression *>(current_node));
                break;
            case binding::BoundNodeKind::CastExpression:
                lower(*static_cast<const binding::BoundCastExpression *>(current_node));
                break;
            case binding::BoundNodeKind::ParenthesizedExpression:
                lower(*static_cast<const binding::BoundParenthesizedExpression *>(current_node));
                break;
            case binding::BoundNodeKind::IdentifierExpression:
                lower(*static_cast<const binding::BoundIdentifierExpression *>(current_node));
                break;
            case binding::BoundNodeKind::AssignmentExpression:
                lower(*static_cast<const binding::BoundAssignmentExpression *>(current_node));
                break;
            case binding::BoundNodeKind::CallExpression:
                lower(*static_cast<const binding::BoundCallExpression *>(current_node));
                break;
            default:
                SCC_NOT_IMPLEMENTED("Unknown BoundNodeKind in Lowerer::lower");
            }
        }

        return result;
    }
}