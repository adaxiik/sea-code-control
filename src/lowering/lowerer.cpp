#include "lowering/lowerer.hpp"
#include "cpp_compiler.hpp"

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
        SCC_NOT_IMPLEMENTED("BoundExpressionStatement");
    }
    void Lowerer::lower(const binding::BoundBlockStatement &block_statement)
    {
        SCC_NOT_IMPLEMENTED("BoundBlockStatement");
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
        SCC_NOT_IMPLEMENTED("BoundBinaryExpression");
    }
    void Lowerer::lower(const binding::BoundLiteralExpression &literal_expression)
    {
        SCC_NOT_IMPLEMENTED("BoundLiteralExpression");
    }
    void Lowerer::lower(const binding::BoundCastExpression &cast_expression)
    {
        SCC_NOT_IMPLEMENTED("BoundCastExpression");
    }
    void Lowerer::lower(const binding::BoundParenthesizedExpression &parenthesized_expression)
    {
        SCC_NOT_IMPLEMENTED("BoundParenthesizedExpression");
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

    std::vector<std::unique_ptr<lowering::Instruction>> Lowerer::lower(const std::unique_ptr<binding::BoundNode> &root)
    {
        m_result.clear();
        m_to_lower.push(root.get());

        while (!m_to_lower.empty())
        {
            auto current_node = m_to_lower.top();
            m_to_lower.pop();
            static_assert(static_cast<int>(binding::BoundNodeKind::COUNT) == 17);
            switch (current_node->bound_node_kind())
            {
            case binding::BoundNodeKind::ExpressionStatement:
                lower(*static_cast<binding::BoundExpressionStatement *>(current_node));
                break;
            case binding::BoundNodeKind::BlockStatement:
                lower(*static_cast<binding::BoundBlockStatement *>(current_node));
                break;
            case binding::BoundNodeKind::VariableDeclarationStatement:
                lower(*static_cast<binding::BoundVariableDeclarationStatement *>(current_node));
                break;
            case binding::BoundNodeKind::IfStatement:
                lower(*static_cast<binding::BoundIfStatement *>(current_node));
                break;
            case binding::BoundNodeKind::WhileStatement:
                lower(*static_cast<binding::BoundWhileStatement *>(current_node));
                break;
            case binding::BoundNodeKind::DoStatement:
                lower(*static_cast<binding::BoundDoStatement *>(current_node));
                break;
            case binding::BoundNodeKind::BreakStatement:
                lower(*static_cast<binding::BoundBreakStatement *>(current_node));
                break;
            case binding::BoundNodeKind::ContinueStatement:
                lower(*static_cast<binding::BoundContinueStatement *>(current_node));
                break;
            case binding::BoundNodeKind::FunctionStatement:
                lower(*static_cast<binding::BoundFunctionStatement *>(current_node));
                break;
            case binding::BoundNodeKind::ReturnStatement:
                lower(*static_cast<binding::BoundReturnStatement *>(current_node));
                break;
            case binding::BoundNodeKind::BinaryExpression:
                lower(*static_cast<binding::BoundBinaryExpression *>(current_node));
                break;
            case binding::BoundNodeKind::LiteralExpression:
                lower(*static_cast<binding::BoundLiteralExpression *>(current_node));
                break;
            case binding::BoundNodeKind::CastExpression:
                lower(*static_cast<binding::BoundCastExpression *>(current_node));
                break;
            case binding::BoundNodeKind::ParenthesizedExpression:
                lower(*static_cast<binding::BoundParenthesizedExpression *>(current_node));
                break;
            case binding::BoundNodeKind::IdentifierExpression:
                lower(*static_cast<binding::BoundIdentifierExpression *>(current_node));
                break;
            case binding::BoundNodeKind::AssignmentExpression:
                lower(*static_cast<binding::BoundAssignmentExpression *>(current_node));
                break;
            case binding::BoundNodeKind::CallExpression:
                lower(*static_cast<binding::BoundCallExpression *>(current_node));
                break;
            default:
                SCC_NOT_IMPLEMENTED("Unknown BoundNodeKind in Lowerer::lower");
            }
        }

        std::vector<std::unique_ptr<lowering::Instruction>> result;
        for (auto &instruction : m_result)
            result.push_back(std::move(instruction));
        m_result.clear();
        return result;
    }
}