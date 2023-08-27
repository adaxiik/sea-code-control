#include "interpreter.hpp"

#include <iostream>
#include "debug.hpp"
#include "cpp_compiler.hpp"
#include "operation_result.hpp"
#include "overloaded.hpp"

namespace scc
{
    ParserResult Interpreter::parse(const std::string &code)
    {
        return m_parser.parse(code);
    }

    InterpreterResult Interpreter::interpret(const std::string &code)
    {
        auto parse_result = m_parser.parse(code);
        return interpret(parse_result);
    }

    InterpreterResult Interpreter::interpret(const ParserResult &parse_result)
    {
        if (parse_result.has_error())
            return InterpreterError::ParseError;

        auto binded {Binder::bind(parse_result.root_node())};
        if (binded.is_error())
            return InterpreterError::BindError;

        if (binded.get_value()->bound_node_kind() != binding::BoundNodeKind::BlockStatement)
            return InterpreterError::BindError;
        
        auto& block_statement = ikwid_rc<binding::BoundBlockStatement>(ikwid_rc<binding::BoundStatement>(*binded.get_value()));    
        debug::bound_ast_as_text_tree(std::cout, block_statement);
   
        if(block_statement.statements.size() != 1)
            return InterpreterError::BindError;

        TRACE();
        return interpret(*block_statement.statements[0]);
    }

    InterpreterResult Interpreter::interpret(const binding::BoundBlockStatement &block_statement)
    {
        if(block_statement.statements.size() == 1)
        {
            m_scope_stack.push();
            auto result = interpret(*block_statement.statements[0]);
            m_scope_stack.pop();
            return result;
        }

        m_scope_stack.push();
        for (const auto& statement : block_statement.statements)
        {
            auto result = interpret(*statement);
            if (result.is_error())
                return result;
        }
        m_scope_stack.pop(); 

        return InterpreterError::None;
    }

    InterpreterResult Interpreter::interpret(const binding::BoundVariableDeclarationStatement &variable_declaration)
    {
        static_assert(static_cast<int>(binding::BoundVariableDeclarationStatement::VariableDeclarationStatementKind::COUNT)  == 3, "Update this code");
        TRACE();

        auto creation_result = m_scope_stack.create_variable(variable_declaration.variable_name
                                                           , variable_declaration.type
                                                           , variable_declaration.size_bytes()
                                                           , variable_declaration.is_constant);
        if (creation_result.is_error())
            return creation_result;

        switch (variable_declaration.variable_declaration_statement_kind())
        {
        case binding::BoundVariableDeclarationStatement::VariableDeclarationStatementKind::Value:
        {
            auto& vd = ikwid_rc<binding::BoundVariableValueDeclarationStatement>(variable_declaration);
            if (!vd.initializer)
                return creation_result;

            return interpret(vd);
        }
        case binding::BoundVariableDeclarationStatement::VariableDeclarationStatementKind::Pointer:
        {
            auto& vd = ikwid_rc<binding::BoundVariablePointerDeclarationStatement>(variable_declaration);
            if (!vd.initializer)
                return creation_result;

            return interpret(vd);
        }
        case binding::BoundVariableDeclarationStatement::VariableDeclarationStatementKind::StaticArray:
        {
            auto& vd = ikwid_rc<binding::BoundVariableStaticArrayDeclarationStatement>(variable_declaration);
            if (vd.initializers.empty())
                return creation_result;

            return interpret(vd);
        }
        default:
            // UNREACHABLE
            return InterpreterError::ReachedUnreachableError;
        }
        return InterpreterError::RuntimeError;
    }

    InterpreterResult Interpreter::interpret(const binding::BoundVariableValueDeclarationStatement &variable_value_declaration)
    {
        TRACE();
        auto result = eval(*variable_value_declaration.initializer);
        if (result.is_error())
            return result;
        
        auto variable = m_scope_stack.get_from_scopestack(variable_value_declaration.variable_name);
        if (!variable)
            return InterpreterError::VariableDoesntExistError; // probably unreachable
        
        if (variable->set_value(m_memory, result.get_value().value))
            return InterpreterResult::ok(variable->address());

        return InterpreterError::RuntimeError;
    }

    InterpreterResult Interpreter::interpret(const binding::BoundVariablePointerDeclarationStatement &variable_pointer_declaration)
    {
        TRACE();
        // TODOOOOOOOOOOOOOOOOOOOO:
        // initializer must be a identifer expression?
        
        return InterpreterError::RuntimeError;
    }

    InterpreterResult Interpreter::interpret(const binding::BoundVariableStaticArrayDeclarationStatement &variable_reference_declaration)
    {
        TRACE();
        // TODOOOOOOOOOOOOOOOOOOOO:
        
        return InterpreterError::RuntimeError;
    }

    InterpreterResult Interpreter::interpret(const binding::BoundStatement &statement)
    {
        TRACE();
        static_assert(binding::STATEMENT_COUNT == 3, "Update this code");
        switch (statement.bound_node_kind())
        {
        case binding::BoundNodeKind::ExpressionStatement:
            return eval(*ikwid_rc<binding::BoundExpressionStatement>(statement).expression);
        case binding::BoundNodeKind::BlockStatement:
            return interpret(ikwid_rc<binding::BoundBlockStatement>(statement));
        case binding::BoundNodeKind::VariableDeclarationStatement:
            return interpret(ikwid_rc<binding::BoundVariableDeclarationStatement>(statement));
        default:
            // UNREACHABLE
            return InterpreterError::BindError;
        }
    }

    InterpreterResult Interpreter::eval(const binding::BoundLiteralExpression &literal_expression)
    {
        TRACE();
        return InterpreterResult::ok(literal_expression.value);
    }

    InterpreterResult Interpreter::eval(const binding::BoundAssignmentExpression &assignment_expression)
    {
        TRACE();
        auto result = eval(*assignment_expression.expression);
        if (result.is_error())
            return result;

        auto variable = m_scope_stack.get_from_scopestack(assignment_expression.identifier);
        if (!variable)
            return InterpreterError::VariableDoesntExistError;

        if (variable->set_value(m_memory, result.get_value().value))
            return result;

        return InterpreterError::RuntimeError;
    }


    InterpreterResult Interpreter::eval(const binding::BoundExpression& expression)
    {
        TRACE();
        static_assert(binding::EXPRESSION_COUNT == 6, "Update this code");
        switch (expression.bound_node_kind())
        {
        case binding::BoundNodeKind::BinaryExpression:
            return eval(ikwid_rc<binding::BoundBinaryExpression>(expression));
        case binding::BoundNodeKind::LiteralExpression:
            return eval(ikwid_rc<binding::BoundLiteralExpression>(expression));
        case binding::BoundNodeKind::CastExpression:
            return eval(ikwid_rc<binding::BoundCastExpression>(expression));
        case binding::BoundNodeKind::ParenthesizedExpression:
            return eval(ikwid_rc<binding::BoundParenthesizedExpression>(expression));
        case binding::BoundNodeKind::IdentifierExpression:
            return eval(ikwid_rc<binding::BoundIdentifierExpression>(expression));
        case binding::BoundNodeKind::AssignmentExpression:
            return eval(ikwid_rc<binding::BoundAssignmentExpression>(expression));
        default:
            // UNREACHABLE
            return InterpreterResult::error(InterpreterError::ReachedUnreachableError);
        }
    }

    InterpreterResult Interpreter::eval(const binding::BoundParenthesizedExpression &parenthesized_expression)
    {
        TRACE();
        for (size_t i = 0; i < parenthesized_expression.expressions.size(); i++)
        {
            auto result{eval(*parenthesized_expression.expressions[i])};
            if (result.is_error())
                return result;
            
            if (i == parenthesized_expression.expressions.size() - 1)
                return result;
        }

        // UNREACHABLE
        return InterpreterResult::error(InterpreterError::ReachedUnreachableError);
    }

    InterpreterResult Interpreter::eval(const binding::BoundIdentifierExpression &identifier_expression)
    {
        TRACE();
        auto variable = m_scope_stack.get_from_scopestack(identifier_expression.identifier);
        if (!variable)
            return InterpreterError::VariableDoesntExistError;

        if (!variable->is_initialized())
            return InterpreterError::VariableNotInitializedError;
        
        
        auto value = variable->get_value(m_memory, variable->type());
        if (!value)
            return InterpreterError::VariableNotInitializedError; // or memory error??

        return InterpreterResult::ok(InterpreterResultValue(value.value()));
    }
}
