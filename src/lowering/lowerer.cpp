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
        // m_to_lower.push(std::make_pair(lowering::PopScopeInstruction(), block_statement.location));
        m_to_lower.push(std::make_pair(lowering::PopScopeInstruction(), block_statement.location));

        for (auto it = block_statement.statements.rbegin(); it != block_statement.statements.rend(); it++)
        {
            auto node = (*it).get();
            if (should_drop_after_statement(node))
            {
                m_to_lower.push(std::make_pair(lowering::DropInstruction(), node->location));
            }
            m_to_lower.push(node);
        }

        m_to_lower.push(std::make_pair(lowering::PushScopeInstruction(), block_statement.location));
    }

    void Lowerer::lower(const binding::BoundVariableValueDeclarationStatement &variable_value_declaration_statement)
    {
        using CVFlags = lowering::CreateValueVariableInstruction::Flags;
        m_to_lower.push(std::make_pair(lowering::CreateValueVariableInstruction(
            variable_value_declaration_statement.variable_name,
            variable_value_declaration_statement.type,
            CVFlags::None
            | ((variable_value_declaration_statement.initializer != nullptr) ? CVFlags::HasInitializer : CVFlags::None)
            | (variable_value_declaration_statement.is_global ? CVFlags::IsGlobal : CVFlags::None)
            | (variable_value_declaration_statement.is_constant ? CVFlags::IsConst : CVFlags::None)
        ), variable_value_declaration_statement.location));

        if (variable_value_declaration_statement.initializer)
        {
            m_to_lower.push(variable_value_declaration_statement.initializer.get());
        }
        // else if (variable_value_declaration_statement.is_global)
        // {
        //     m_to_lower.push(std::make_pair(lowering::CastInstruction(variable_value_declaration_statement.type), variable_value_declaration_statement.location));
        //     m_to_lower.push(std::make_pair(lowering::PushLiteralInstruction(Type::PrimitiveType::I32, 0), variable_value_declaration_statement.location));
        // }

    }

    void Lowerer::lower(const binding::BoundVariablePointerDeclarationStatement &variable_pointer_declaration_statement)
    {
        using CVFlags = lowering::CreateValueVariableInstruction::Flags;
        m_to_lower.push(std::make_pair(lowering::CreateValueVariableInstruction(
            variable_pointer_declaration_statement.variable_name,
            variable_pointer_declaration_statement.type,
            CVFlags::None
            | ((variable_pointer_declaration_statement.initializer != nullptr or variable_pointer_declaration_statement.is_global) ? CVFlags::HasInitializer : CVFlags::None)
            | (variable_pointer_declaration_statement.is_global ? CVFlags::IsGlobal : CVFlags::None)
            | (variable_pointer_declaration_statement.is_constant ? CVFlags::IsConst : CVFlags::None)
        ), variable_pointer_declaration_statement.location));

        if (variable_pointer_declaration_statement.initializer)
        {
            m_to_lower.push(variable_pointer_declaration_statement.initializer.get());
        }
        else if (variable_pointer_declaration_statement.is_global)
        {
            m_to_lower.push(std::make_pair(lowering::CastInstruction(variable_pointer_declaration_statement.type), variable_pointer_declaration_statement.location));
            m_to_lower.push(std::make_pair(lowering::PushLiteralInstruction(Type::PrimitiveType::I32, 0), variable_pointer_declaration_statement.location));
        }
    }

    void Lowerer::lower(const binding::BoundVariableStaticArrayDeclarationStatement &variable_static_array_declaration_statement)
    {
        using CAFlags = lowering::CreateArrayVariableInstruction::Flags;
        m_to_lower.push(std::make_pair(lowering::CreateArrayVariableInstruction(
            variable_static_array_declaration_statement.variable_name,
            variable_static_array_declaration_statement.type,
            CAFlags::None
            | ((variable_static_array_declaration_statement.initializers.size() > 0 or variable_static_array_declaration_statement.is_global) ? CAFlags::HasInitializer : CAFlags::None)
            | (variable_static_array_declaration_statement.is_global ? CAFlags::IsGlobal : CAFlags::None)
            | (variable_static_array_declaration_statement.is_constant ? CAFlags::IsConst : CAFlags::None),
            variable_static_array_declaration_statement.initializers.size()
        ), variable_static_array_declaration_statement.location));

        for (auto& initializer: variable_static_array_declaration_statement.initializers)
            m_to_lower.push(initializer.get());


    }

    void Lowerer::lower(const binding::BoundVariableDeclarationStatement &variable_declaration_statement)
    {
        switch (variable_declaration_statement.variable_declaration_statement_kind())
        {
            using DeclarationType = binding::BoundVariableDeclarationStatement::VariableDeclarationStatementKind;
        case DeclarationType::Value:
            lower(*static_cast<const binding::BoundVariableValueDeclarationStatement*>(&variable_declaration_statement));
            break;
        case DeclarationType::Pointer:
            lower(*static_cast<const binding::BoundVariablePointerDeclarationStatement*>(&variable_declaration_statement));
            break;
        case DeclarationType::StaticArray:
            lower(*static_cast<const binding::BoundVariableStaticArrayDeclarationStatement*>(&variable_declaration_statement));
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
            m_to_lower.push(std::make_pair(lowering::LabelInstruction(end_label), if_statement.location));
            m_to_lower.push(if_statement.then_statement.get());
            m_to_lower.push(std::make_pair(lowering::GotoFalseInstruction(end_label), if_statement.location));
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
        m_to_lower.push(std::make_pair(lowering::LabelInstruction(end_label), if_statement.location));
        m_to_lower.push(if_statement.else_statement.get());
        m_to_lower.push(std::make_pair(lowering::LabelInstruction(else_label), if_statement.location));
        m_to_lower.push(std::make_pair(lowering::GotoInstruction(end_label), if_statement.location));
        m_to_lower.push(if_statement.then_statement.get());
        m_to_lower.push(std::make_pair(lowering::GotoFalseInstruction(else_label), if_statement.location));
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

        m_to_lower.push(std::make_pair(lowering::LabelInstruction(break_label), while_statement.location));
        m_to_lower.push(std::make_pair(lowering::GotoInstruction(continue_label), while_statement.location));
        if (while_statement.body)
            m_to_lower.push(while_statement.body.get());
        m_to_lower.push(std::make_pair(lowering::GotoFalseInstruction(break_label), while_statement.location));
        m_to_lower.push(while_statement.condition.get());
        m_to_lower.push(std::make_pair(lowering::LabelInstruction(continue_label), while_statement.location));

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

        m_to_lower.push(std::make_pair(lowering::LabelInstruction(break_label), do_statement.location));
        m_to_lower.push(std::make_pair(lowering::GotoTrueInstruction(continue_label), do_statement.location));
        m_to_lower.push(do_statement.condition.get());
        if (do_statement.body)
            m_to_lower.push(do_statement.body.get());
        m_to_lower.push(std::make_pair(lowering::LabelInstruction(continue_label), do_statement.location));

        m_to_lower.push(PushLabels(continue_label, break_label));
    }

    void Lowerer::lower(const binding::BoundBreakStatement & break_statement)
    {
        if (m_loop_labels.empty())
            SCC_UNREACHABLE();

        m_to_lower.push(std::make_pair(lowering::GotoInstruction(m_loop_labels.top().break_label), break_statement.location));
        m_to_lower.push(std::make_pair(lowering::PopScopeInstruction(), break_statement.location));
    }

    void Lowerer::lower(const binding::BoundContinueStatement & continue_statement)
    {
        if (m_loop_labels.empty())
            SCC_UNREACHABLE();

        m_to_lower.push(std::make_pair(lowering::GotoInstruction(m_loop_labels.top().continue_label), continue_statement.location));
        m_to_lower.push(std::make_pair(lowering::PopScopeInstruction(), continue_statement.location));
    }

    void Lowerer::lower(const binding::BoundFunctionStatement &function_statement)
    {
        if (!function_statement.body)
            return;

        Label funtion_end_label = create_label();

        m_to_lower.push(std::make_pair(lowering::LabelInstruction(funtion_end_label), function_statement.location));
        m_to_lower.push(std::make_pair(lowering::PopScopeInstruction(), function_statement.location));

        // for missing returns in non void functions should check binder
        if (function_statement.return_type == Type(Type::PrimitiveType::Void))
            m_to_lower.push(std::make_pair(lowering::ReturnInstruction(), function_statement.location));


        auto& statements = function_statement.body->statements;
        for (auto it = statements.rbegin(); it != statements.rend(); it++)
        {
            auto node = (*it).get();
            if (should_drop_after_statement(node))
            {
                m_to_lower.push(std::make_pair(lowering::DropInstruction(), node->location));
            }
            m_to_lower.push(node);
        }

        for (auto it = function_statement.parameters.rbegin(); it != function_statement.parameters.rend(); it++)
        {
            m_to_lower.push(std::make_pair(lowering::DropInstruction(), function_statement.location));
            auto& parameter = *it;
            using CVFlags = lowering::CreateValueVariableInstruction::Flags;
            m_to_lower.push(std::make_pair(lowering::CreateValueVariableInstruction(
                parameter->variable_name,
                parameter->type,
                CVFlags::HasInitializer | CVFlags::IsParameter | (parameter->is_constant ? CVFlags::IsConst : CVFlags::None)
            ), function_statement.location));
        }

        m_to_lower.push(std::make_pair(lowering::PushScopeInstruction(), function_statement.location));
        m_to_lower.push(std::make_pair(lowering::RegisterFunctionInstruction(function_statement.function_name), function_statement.location));
        m_to_lower.push(std::make_pair(lowering::GotoInstruction(funtion_end_label), function_statement.location));
    }

    void Lowerer::lower(const binding::BoundReturnStatement &return_statement)
    {
        m_to_lower.push(std::make_pair(lowering::ReturnInstruction(return_statement.has_return_expression()), return_statement.location));

        if (return_statement.has_return_expression())
            m_to_lower.push(return_statement.return_expression.get());
    }

    void Lowerer::lower(const binding::BoundBinaryExpression &binary_expression)
    {
        m_to_lower.push(std::make_pair(lowering::BinaryOperationInstruction(binary_expression.op_kind), binary_expression.location));
        m_to_lower.push(binary_expression.left.get());
        m_to_lower.push(binary_expression.right.get());
    }

    void Lowerer::lower(const binding::BoundLiteralExpression &literal_expression)
    {
        m_to_lower.push(std::make_pair(lowering::PushLiteralInstruction(literal_expression.type, literal_expression.value), literal_expression.location));
    }

    void Lowerer::lower(const binding::BoundCastExpression &cast_expression)
    {
        m_to_lower.push(std::make_pair(lowering::CastInstruction(cast_expression.type), cast_expression.location));
        m_to_lower.push(cast_expression.expression.get());
    }

    void Lowerer::lower(const binding::BoundParenthesizedExpression &parenthesized_expression)
    {
        if (parenthesized_expression.expressions.size() > 1)
            m_to_lower.push(std::make_pair(lowering::DropInstruction(parenthesized_expression.expressions.size() - 1), parenthesized_expression.location));

        for (const auto& expression: parenthesized_expression.expressions)
            m_to_lower.push(expression.get());
    }

    void Lowerer::lower(const binding::BoundIdentifierExpression &identifier_expression)
    {
        m_to_lower.push(std::make_pair(lowering::IdentifierInstruction(identifier_expression.identifier, identifier_expression.type), identifier_expression.location));
    }

    void Lowerer::lower(const binding::BoundAssignmentExpression &assignment_expression)
    {
        m_to_lower.push(std::make_pair(lowering::AssignmentInstruction(assignment_expression.identifier), assignment_expression.location));
        m_to_lower.push(assignment_expression.expression.get());
    }

    void Lowerer::lower_user_fn_call(const binding::BoundCallExpression &call_expression)
    {
        m_to_lower.push(std::make_pair(lowering::CallInstruction(call_expression.function_name, call_expression.type != Type(Type::PrimitiveType::Void)), call_expression.location));
        for (auto& argument: call_expression.arguments)
            m_to_lower.push(argument.get());
    }

    void Lowerer::lower_inbuild_fn_call(const binding::BoundCallExpression &call_expression)
    {
        const auto& fn = lowering::inbuild::inbuild_functions.at(call_expression.function_name);

        m_to_lower.push(
            std::make_pair(
                lowering::CallInbuildInstruction(
                    call_expression.function_name,
                    fn.inbuild_function,
                    fn.return_type != Type(Type::PrimitiveType::Void)
                ),
            call_expression.location)
        );

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

        m_to_lower.push(std::make_pair(lowering::PopScopeInstruction(), for_statement.location));
        m_to_lower.push(PopLabels());

        m_to_lower.push(std::make_pair(lowering::LabelInstruction(break_label), for_statement.location));

        m_to_lower.push(std::make_pair(lowering::GotoInstruction(restart_label), for_statement.location));

        if (for_statement.increment)
        {
            // we need to drop after expression
            m_to_lower.push(std::make_pair(lowering::DropInstruction(), for_statement.location));
            m_to_lower.push(for_statement.increment.get());
        }

        m_to_lower.push(std::make_pair(lowering::LabelInstruction(continue_label), for_statement.location));

        if (for_statement.body)
            m_to_lower.push(for_statement.body.get());

        if (for_statement.condition)
        {
            m_to_lower.push(std::make_pair(lowering::GotoFalseInstruction(break_label), for_statement.location));
            m_to_lower.push(for_statement.condition.get());
        }

        m_to_lower.push(std::make_pair(lowering::LabelInstruction(restart_label), for_statement.location));

        if (for_statement.initializer)
            m_to_lower.push(for_statement.initializer.get());

        m_to_lower.push(PushLabels(continue_label, break_label));
        m_to_lower.push(std::make_pair(lowering::PushScopeInstruction(), for_statement.location));

    }

    void Lowerer::lower(const binding::BoundReferenceExpression &reference_expression)
    {
        m_to_lower.push(std::make_pair(lowering::ReferenceInstruction(reference_expression.identifier), reference_expression.location));
    }

    void Lowerer::lower(const binding::BoundDereferenceExpression &dereference_expression)
    {
        m_to_lower.push(std::make_pair(lowering::DereferenceInstruction(dereference_expression.type), dereference_expression.location));
        m_to_lower.push(dereference_expression.expression.get());
    }

    void Lowerer::lower(const binding::BoundPointerAssignmentExpression &pointer_assignment_expression)
    {
        m_to_lower.push(std::make_pair(lowering::PointerAssignmentInstruction(pointer_assignment_expression.type), pointer_assignment_expression.location));
        m_to_lower.push(pointer_assignment_expression.expression.get());
        m_to_lower.push(pointer_assignment_expression.address_expression.get());
    }

    void Lowerer::lower(const binding::BoundUnaryExpression &unary_expression)
    {
        m_to_lower.push(std::make_pair(lowering::UnaryOperationInstruction(unary_expression.op_kind), unary_expression.location));
        m_to_lower.push(unary_expression.expression.get());
    }

    void Lowerer::lower(const binding::BoundStringExpression &string_expression)
    {
        m_to_lower.push(std::make_pair(lowering::PushStringInstruction(string_expression.value), string_expression.location));
    }

    void Lowerer::lower(const binding::BoundPrintfExpression &printf_expression)
    {
        m_to_lower.push(std::make_pair(lowering::PrintfInstruction(printf_expression.arguments.size()), printf_expression.location));

        m_to_lower.push(printf_expression.format.get());
        for (auto& argument: printf_expression.arguments)
            m_to_lower.push(argument.get());

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
            static_cast<const binding::BoundCallExpression*>(expression)->type == Type(Type::PrimitiveType::Void))
        {
            return false;
        }

        return true;
    }

    Lowerer::Label Lowerer::create_label()
    {
        return m_current_label++;
    }

    std::vector<std::pair<lowering::Instruction, std::optional<Location>>> Lowerer::lower(const binding::BoundBlockStatement *root)
    {
        auto& statements = root->statements;
        for (auto it = statements.rbegin(); it != statements.rend(); it++)
        {
            auto node = it->get();
            if (statements.size() > 1 &&
                should_drop_after_statement(node))
            {
                m_to_lower.push(std::make_pair(lowering::DropInstruction(), node->location));
            }

            m_to_lower.push(node);
        }

        std::vector<std::pair<lowering::Instruction, std::optional<Location>>> result;

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

            static_assert(static_cast<int>(binding::BoundNodeKind::COUNT) == 24);

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
            case binding::BoundNodeKind::ReferenceExpression:
                lower(*static_cast<const binding::BoundReferenceExpression *>(current_node));
                break;
            case binding::BoundNodeKind::DereferenceExpression:
                lower(*static_cast<const binding::BoundDereferenceExpression *>(current_node));
                break;
            case binding::BoundNodeKind::PointerAssignmentExpression:
                lower(*static_cast<const binding::BoundPointerAssignmentExpression *>(current_node));
                break;
            case binding::BoundNodeKind::UnaryExpression:
                lower(*static_cast<const binding::BoundUnaryExpression *>(current_node));
                break;
            case binding::BoundNodeKind::StringExpression:
                lower(*static_cast<const binding::BoundStringExpression *>(current_node));
                break;
            case binding::BoundNodeKind::PrintfExpression:
                lower(*static_cast<const binding::BoundPrintfExpression *>(current_node));
                break;
            default:
                SCC_NOT_IMPLEMENTED("Unknown BoundNodeKind in Lowerer::lower");
            }
        }

        return result;
    }
}