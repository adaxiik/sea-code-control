#include "lowering/lowerer.hpp"
#include "cpp_compiler.hpp"

#include "lowering/push_literal_instruction.hpp"
#include "lowering/pop_scope_instruction.hpp"
#include "lowering/push_scope_instruction.hpp"
#include "lowering/binary_operation_instruction.hpp"
#include "lowering/cast_instruction.hpp"
#include "lowering/drop_instruction.hpp"
#include "lowering/identifier_instruction.hpp"
#include "lowering/inbuild_functions.hpp"

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

#define SCC_UNREACHABLE()                                                \
    do                                                                   \
    {                                                                    \
        std::cerr << "==============================" << std::endl;      \
        std::cerr << "Unreachable code" << std::endl;                    \
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
        m_to_lower.push(lowering::PopScopeInstruction());

        for (auto it = block_statement.statements.rbegin(); it != block_statement.statements.rend(); it++)
        {
            auto node = (*it).get();
            if (should_drop_after_statement(node))
            {
                m_to_lower.push(lowering::DropInstruction());
            }
            m_to_lower.push(node);
        }

        m_to_lower.push(lowering::PushScopeInstruction());
    }

    void Lowerer::lower(const binding::BoundVariableValueDeclarationStatement &variable_value_declaration_statement)
    {
        m_to_lower.push(lowering::CreateValueVariableInstruction(
            variable_value_declaration_statement.variable_name,
            variable_value_declaration_statement.type,
            variable_value_declaration_statement.initializer != nullptr,
            variable_value_declaration_statement.is_constant,
            variable_value_declaration_statement.is_global
        ));
        
        if (variable_value_declaration_statement.initializer)
            m_to_lower.push(variable_value_declaration_statement.initializer.get());
    }

    void Lowerer::lower(const binding::BoundVariableDeclarationStatement &variable_declaration_statement)
    {
        switch (variable_declaration_statement.variable_declaration_statement_kind())
        {
            using DeclarationType = binding::BoundVariableDeclarationStatement::VariableDeclarationStatementKind;
        case DeclarationType::Value:
            lower(*static_cast<const binding::BoundVariableValueDeclarationStatement*>(&variable_declaration_statement));
            break;
        default:
            SCC_NOT_IMPLEMENTED("BoundVariableDeclarationStatement");
            break;
        }
    }

    void Lowerer::lower(const binding::BoundIfStatement &if_statement)
    {
        if (!if_statement.else_statement)
        {
            // if (condition) { ... }
            //   =>
            // condition
            // goto_false end
            // ...
            // end:

            Label end_label = create_label();
            m_to_lower.push(lowering::LabelInstruction(end_label));
            m_to_lower.push(if_statement.then_statement.get());
            m_to_lower.push(lowering::GotoFalseInstruction(end_label));
            m_to_lower.push(if_statement.condition.get());
            return;
        }
        
        // if (condition) { ... } else { ... }
        //   =>
        // condition
        // goto_false else
        // ...
        // goto end
        // else:
        // ...
        // end:

        Label else_label = create_label();
        Label end_label = create_label();
        m_to_lower.push(lowering::LabelInstruction(end_label));
        m_to_lower.push(if_statement.else_statement.get());
        m_to_lower.push(lowering::LabelInstruction(else_label));
        m_to_lower.push(lowering::GotoInstruction(end_label));
        m_to_lower.push(if_statement.then_statement.get());
        m_to_lower.push(lowering::GotoFalseInstruction(else_label));
        m_to_lower.push(if_statement.condition.get());
    }

    void Lowerer::lower(const binding::BoundWhileStatement &while_statement)
    {
        // while (condition) { ... }
        //   =>
        // continue:
        // condition
        // goto_false break
        // ...
        // goto continue
        // break:

        Label continue_label = create_label();
        Label break_label = create_label();

        m_to_lower.push(PopLabels());
      
        m_to_lower.push(lowering::LabelInstruction(break_label));
        m_to_lower.push(lowering::GotoInstruction(continue_label));
        if (while_statement.body)
            m_to_lower.push(while_statement.body.get());
        m_to_lower.push(lowering::GotoFalseInstruction(break_label));
        m_to_lower.push(while_statement.condition.get());
        m_to_lower.push(lowering::LabelInstruction(continue_label));
        
        m_to_lower.push(PushLabels(continue_label, break_label));
    }

    void Lowerer::lower(const binding::BoundDoStatement &do_statement)
    {
        // do { ... } while (condition)
        //   =>
        // continue:
        // ...
        // condition
        // goto_true continue
        // break:

        Label continue_label = create_label();
        Label break_label = create_label();

        m_to_lower.push(PopLabels());

        m_to_lower.push(lowering::LabelInstruction(break_label));
        m_to_lower.push(lowering::GotoTrueInstruction(continue_label));
        m_to_lower.push(do_statement.condition.get());
        if (do_statement.body)
            m_to_lower.push(do_statement.body.get());
        m_to_lower.push(lowering::LabelInstruction(continue_label));

        m_to_lower.push(PushLabels(continue_label, break_label));
    }

    void Lowerer::lower(const binding::BoundBreakStatement &)
    {
        if (m_loop_labels.empty())
            SCC_UNREACHABLE();
        
        m_to_lower.push(lowering::GotoInstruction(m_loop_labels.top().break_label));
        m_to_lower.push(lowering::PopScopeInstruction());
    }

    void Lowerer::lower(const binding::BoundContinueStatement &)
    {
        if (m_loop_labels.empty())
            SCC_UNREACHABLE();
        
        m_to_lower.push(lowering::GotoInstruction(m_loop_labels.top().continue_label));
        m_to_lower.push(lowering::PopScopeInstruction());
    }

    void Lowerer::lower(const binding::BoundFunctionStatement &function_statement)
    {
        if (!function_statement.body)
            return;
        
        Label funtion_end_label = create_label();

        m_to_lower.push(lowering::LabelInstruction(funtion_end_label));
        m_to_lower.push(lowering::PopScopeInstruction());

        // for missing returns in non void functions should check binder
        if (function_statement.return_type == Type(Type::Kind::Void))
            m_to_lower.push(lowering::ReturnInstruction());


        auto& statements = function_statement.body->statements;
        for (auto it = statements.rbegin(); it != statements.rend(); it++)
        {
            auto node = (*it).get();
            if (should_drop_after_statement(node))
            {
                m_to_lower.push(lowering::DropInstruction());
            }
            m_to_lower.push(node);
        }

        for (auto it = function_statement.parameters.rbegin(); it != function_statement.parameters.rend(); it++)
        {
            m_to_lower.push(lowering::DropInstruction());
            auto& parameter = *it;
            m_to_lower.push(lowering::CreateValueVariableInstruction(
                parameter->variable_name,
                parameter->type,
                true,
                parameter->is_constant,
                false
            ));
        }

        m_to_lower.push(lowering::PushScopeInstruction());
        m_to_lower.push(lowering::RegisterFunctionInstruction(function_statement.function_name));
        m_to_lower.push(lowering::GotoInstruction(funtion_end_label));
    }

    void Lowerer::lower(const binding::BoundReturnStatement &return_statement)
    {
        m_to_lower.push(lowering::ReturnInstruction(return_statement.has_return_expression()));

        if (return_statement.has_return_expression())
            m_to_lower.push(return_statement.return_expression.get());
    }

    void Lowerer::lower(const binding::BoundBinaryExpression &binary_expression)
    {
        m_to_lower.push(lowering::BinaryOperationInstruction(binary_expression.op_kind));
        m_to_lower.push(binary_expression.left.get());
        m_to_lower.push(binary_expression.right.get());
    }

    void Lowerer::lower(const binding::BoundLiteralExpression &literal_expression)
    {
        m_to_lower.push(lowering::PushLiteralInstruction(literal_expression.type, literal_expression.value));
    }

    void Lowerer::lower(const binding::BoundCastExpression &cast_expression)
    {
        m_to_lower.push(lowering::CastInstruction(cast_expression.type));
        m_to_lower.push(cast_expression.expression.get());
    }

    void Lowerer::lower(const binding::BoundParenthesizedExpression &parenthesized_expression)
    {
        if (parenthesized_expression.expressions.size() > 1)
            m_to_lower.push(lowering::DropInstruction(parenthesized_expression.expressions.size() - 1));
        
        for (const auto& expression: parenthesized_expression.expressions)
            m_to_lower.push(expression.get());
    }

    void Lowerer::lower(const binding::BoundIdentifierExpression &identifier_expression)
    {
        m_to_lower.push(lowering::IdentifierInstruction(identifier_expression.identifier, identifier_expression.type));
    }

    void Lowerer::lower(const binding::BoundAssignmentExpression &assignment_expression)
    {
        m_to_lower.push(lowering::AssignmentInstruction(assignment_expression.identifier));
        m_to_lower.push(assignment_expression.expression.get());
    }

    void Lowerer::lower_user_fn_call(const binding::BoundCallExpression &call_expression)
    {
        m_to_lower.push(lowering::CallInstruction(call_expression.function_name, call_expression.type != Type(Type::Kind::Void)));
        for (auto& argument: call_expression.arguments)
            m_to_lower.push(argument.get());
    }

    void Lowerer::lower_inbuild_fn_call(const binding::BoundCallExpression &call_expression)
    {
        const auto& fn = lowering::inbuild::inbuild_functions.at(call_expression.function_name);
        m_to_lower.push(lowering::CallInbuildInstruction(call_expression.function_name, fn.inbuild_function, fn.return_type != Type(Type::Kind::Void)));
        for (auto& argument: call_expression.arguments)
            m_to_lower.push(argument.get());
    }

    void Lowerer::lower(const binding::BoundCallExpression &call_expression)
    {

        if (lowering::inbuild::inbuild_functions.find(call_expression.function_name) == lowering::inbuild::inbuild_functions.end())
        {
            // user function
            lower_user_fn_call(call_expression);
            return;
        }

        // inbuild function
        const auto& function_types = lowering::inbuild::inbuild_functions.at(call_expression.function_name);
        if (function_types.return_type != call_expression.type)
        {
            lower_user_fn_call(call_expression);
            return;
        }

        if (function_types.argument_types.size() != call_expression.arguments.size())
        {
            lower_user_fn_call(call_expression);
            return;
        }

        for (size_t i = 0; i < function_types.argument_types.size(); i++)
        {
            if (function_types.argument_types[i] != call_expression.arguments[i]->type)
            {
                lower_user_fn_call(call_expression);
                return;
            }
        }

        lower_inbuild_fn_call(call_expression);  
    }

    void Lowerer::lower(const binding::BoundForStatement &for_statement)
    {
        // for (initializer; condition; increment) { ... } // initializer, condition, increment can be null.. if so, condition is true
        //   =>
        // initializer
        // restart:
        // condition
        // goto_false break
        // ...
        // continue:
        // increment
        // goto restart
        // break:

        Label restart_label = create_label();
        Label continue_label = create_label();
        Label break_label = create_label();

        m_to_lower.push(lowering::PopScopeInstruction());
        m_to_lower.push(PopLabels());

        m_to_lower.push(lowering::LabelInstruction(break_label));
        
        m_to_lower.push(lowering::GotoInstruction(restart_label));

        if (for_statement.increment)
            m_to_lower.push(for_statement.increment.get());
        
        m_to_lower.push(lowering::LabelInstruction(continue_label));
        
        if (for_statement.body)
            m_to_lower.push(for_statement.body.get());
                
        if (for_statement.condition)
        {
            m_to_lower.push(lowering::GotoFalseInstruction(break_label));
            m_to_lower.push(for_statement.condition.get());
        }

        m_to_lower.push(lowering::LabelInstruction(restart_label));
        
        if (for_statement.initializer)
            m_to_lower.push(for_statement.initializer.get());

        m_to_lower.push(PushLabels(continue_label, break_label));
        m_to_lower.push(lowering::PushScopeInstruction());

    }

    bool Lowerer::should_drop_after_statement(const binding::BoundStatement* statement)
    {

        if (statement->bound_node_kind() == binding::BoundNodeKind::VariableDeclarationStatement)
            return true;
        
        if (statement->bound_node_kind() != binding::BoundNodeKind::ExpressionStatement)
            return false;

        auto expression_statement = static_cast<const binding::BoundExpressionStatement*>(statement);
        auto expression = expression_statement->expression.get();
        if (expression->bound_node_kind() == binding::BoundNodeKind::CallExpression &&
            static_cast<const binding::BoundCallExpression*>(expression)->type == Type(Type::Kind::Void))
        {
            return false;    
        }

        return true;
    }

    Lowerer::Label Lowerer::create_label()
    {
        return m_current_label++;
    }


    std::vector<lowering::Instruction> Lowerer::lower(const binding::BoundBlockStatement *root)
    {
        auto& statements = root->statements;
        for (auto it = statements.rbegin(); it != statements.rend(); it++)
        {
            auto node = it->get();
            if (statements.size() > 1 &&
                should_drop_after_statement(node))
            {
                m_to_lower.push(lowering::DropInstruction());
            }
            
            m_to_lower.push(node);
        }

        std::vector<lowering::Instruction> result;

        while (!m_to_lower.empty())
        {
            const auto& current_node_or_instruction = m_to_lower.top();
            if (std::holds_alternative<InstructionType>(current_node_or_instruction))
            {
                result.push_back(std::move(std::get<InstructionType>(current_node_or_instruction)));
                m_to_lower.pop();
                continue;
            }

            if (std::holds_alternative<PushLabels>(current_node_or_instruction))
            {
                auto labels = std::get<PushLabels>(current_node_or_instruction);
                m_loop_labels.push(labels);
                m_to_lower.pop();
                continue;
            }

            if (std::holds_alternative<PopLabels>(current_node_or_instruction))
            {
                m_loop_labels.pop();
                m_to_lower.pop();
                continue;
            }

            static_assert(static_cast<int>(binding::BoundNodeKind::COUNT) == 18);

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
            case binding::BoundNodeKind::ForStatement:
                lower(*static_cast<const binding::BoundForStatement *>(current_node));
                break;
            default:
                SCC_NOT_IMPLEMENTED("Unknown BoundNodeKind in Lowerer::lower");
            }
        }

        return result;
    }
}