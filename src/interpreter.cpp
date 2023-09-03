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

    InterpreterResult Interpreter::register_functions(binding::BoundBlockStatement &block_statement)
    {
        TRACE();

        bool has_main = std::any_of(block_statement.statements.begin(), block_statement.statements.end(), [](const auto& statement)
        {
            return statement->bound_node_kind() == binding::BoundNodeKind::FunctionStatement
                && static_cast<binding::BoundFunctionStatement*>(statement.get())->function_name == MAIN_FUNCTION_NAME;
        });

        if (!has_main)
            return InterpreterError::MissingMainFunctionError;
        
        size_t index = 0;
        while (index < block_statement.statements.size())
        {
            std::unique_ptr<binding::BoundStatement>& current_statement = block_statement.statements[index];
            if (current_statement->bound_node_kind() != binding::BoundNodeKind::FunctionStatement)
            {
                index++;
                continue;
            }
            std::unique_ptr<binding::BoundFunctionStatement> function_statement {std::unique_ptr<binding::BoundFunctionStatement>(static_cast<binding::BoundFunctionStatement*>(current_statement.release()))} ;
            block_statement.statements.erase(block_statement.statements.begin() + index);

            bool already_exists = m_functions.find(function_statement->function_name) != m_functions.end();
            if (already_exists)
            {
                if (!function_statement->body)
                    continue;
                
                auto& existing_function = m_functions[function_statement->function_name];
                if (existing_function->body)
                    return InterpreterError::FunctionAlreadyDefinedError;
                
                existing_function->body = std::move(function_statement->body); // TODOOOO: might not be defined
            }            

            m_functions[function_statement->function_name] = std::move(function_statement);
        }

        return InterpreterError::None;
    }

    InterpreterResult Interpreter::interpret(const ParserResult &parse_result)
    {
        TRACE();
        if (parse_result.has_error())
            return InterpreterError::ParseError;

        auto binded {m_binder.bind(parse_result.root_node())};
        if (binded.is_error())
            return InterpreterError::BindError;

        if (binded.get_value()->bound_node_kind() != binding::BoundNodeKind::BlockStatement)
            return InterpreterError::BindError;
        
        binding::BoundBlockStatement& block_statement = *static_cast<binding::BoundBlockStatement*>(binded.get_value());
        debug::bound_ast_as_text_tree(std::cout, block_statement); // TODOO: move it somewhere else

        bool has_functions = std::any_of(block_statement.statements.begin(), block_statement.statements.end(), [](const auto& statement)
        {
            return statement->bound_node_kind() == binding::BoundNodeKind::FunctionStatement;
        });

        if (has_functions)
        {
            auto result = register_functions(block_statement);
            if (result.is_error())
                return result;
            
            auto main_function = m_functions.find(MAIN_FUNCTION_NAME);
            if (main_function == m_functions.end())
                return InterpreterError::MissingMainFunctionError;  // unreachable
            
            InterpreterResult main_result = interpret(*main_function->second->body);

            if (main_result.has_signal())
                return InterpreterError::UnhandeledSignalError;
            
            return main_result;
        }
        


        for(size_t i = 0; i < block_statement.statements.size(); i++)
        {
            auto& current_statement = block_statement.statements[i];
            if (current_statement->bound_node_kind() == binding::BoundNodeKind::FunctionStatement)
                continue;

            InterpreterResult result = interpret(*current_statement);

            if (result.is_error())
                return result;
            
            if (result.has_signal())
                return InterpreterError::UnhandeledSignalError;
            
            const bool is_last_statement = i == block_statement.statements.size() - 1;
            const bool is_single_statement = block_statement.statements.size() == 1;
            if (is_last_statement && is_single_statement)
                return result;
        }

        return InterpreterError::None;
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
            
            if (result.has_signal())
            {
                m_scope_stack.pop();
                return result;
            }
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

    InterpreterResult Interpreter::interpret(const binding::BoundVariablePointerDeclarationStatement & /* variable_pointer_declaration */ )
    {
        TRACE();
        // TODOOOOOOOOOOOOOOOOOOOO:
        // initializer must be a identifer expression?
        
        return InterpreterError::RuntimeError;
    }

    InterpreterResult Interpreter::interpret(const binding::BoundVariableStaticArrayDeclarationStatement & /* variable_reference_declaration */ )
    {
        TRACE();
        // TODOOOOOOOOOOOOOOOOOOOO:
        
        return InterpreterError::RuntimeError;
    }

    InterpreterResult Interpreter::interpret(const binding::BoundStatement &statement)
    {
        TRACE();
        static_assert(binding::STATEMENT_COUNT == 9, "Update this code");
        switch (statement.bound_node_kind())
        {
        case binding::BoundNodeKind::ExpressionStatement:
            return eval(*ikwid_rc<binding::BoundExpressionStatement>(statement).expression);
        case binding::BoundNodeKind::BlockStatement:
            return interpret(ikwid_rc<binding::BoundBlockStatement>(statement));
        case binding::BoundNodeKind::VariableDeclarationStatement:
            return interpret(ikwid_rc<binding::BoundVariableDeclarationStatement>(statement));
        case binding::BoundNodeKind::IfStatement:
            return interpret(ikwid_rc<binding::BoundIfStatement>(statement));
        case binding::BoundNodeKind::WhileStatement:
            return interpret(ikwid_rc<binding::BoundWhileStatement>(statement));
        case binding::BoundNodeKind::DoStatement:
            return interpret(ikwid_rc<binding::BoundDoStatement>(statement));
        case binding::BoundNodeKind::BreakStatement:
            return interpret(ikwid_rc<binding::BoundBreakStatement>(statement));
        case binding::BoundNodeKind::ContinueStatement:
            return interpret(ikwid_rc<binding::BoundContinueStatement>(statement));
        case binding::BoundNodeKind::FunctionStatement:
            return interpret(ikwid_rc<binding::BoundFunctionStatement>(statement));
        default:
            // UNREACHABLE
            return InterpreterError::BindError;
        }
    }

    InterpreterResult Interpreter::interpret(const binding::BoundIfStatement &if_statement)
    {
        TRACE();
        auto result = eval(*if_statement.condition);
        if (result.is_error())
            return result;

        if (std::get<Type::Primitive::Bool>(result.get_value().value))
            return interpret(*if_statement.then_statement);
        else if (if_statement.else_statement)
            return interpret(*if_statement.else_statement);
        
        return InterpreterError::None;
    }

    InterpreterResult Interpreter::interpret(const binding::BoundBreakStatement &)
    {
        TRACE();
        return InterpreterResult(InterpreterSignal::Break);
    }

    InterpreterResult Interpreter::interpret(const binding::BoundContinueStatement &)
    {
        TRACE();
        return InterpreterResult(InterpreterSignal::Continue);
    }

    InterpreterResult Interpreter::interpret(const binding::BoundWhileStatement &while_statement)
    {
        TRACE();
        auto result = eval(*while_statement.condition);
        if (result.is_error())
            return result;
        
        if (!while_statement.body)
            return InterpreterError::None; // while(0);

        while (std::get<Type::Primitive::Bool>(result.get_value().value))
        {
            auto body_result = interpret(*while_statement.body);
            if (body_result.is_error())
                return body_result;
            
            
            static_assert(static_cast<int>(InterpreterSignal::COUNT) == 3, "Update this code");
            // TODOO: might be a return in the future

            if(body_result.get_signal() == InterpreterSignal::Break) 
                break; 
                
            result = eval(*while_statement.condition);
            if (result.is_error())
                return result;
                           
        }

        return InterpreterError::None;
    }

    InterpreterResult Interpreter::interpret(const binding::BoundDoStatement &do_statement)
    {
        TRACE();
        InterpreterResult result = InterpreterError::None;
        
        do
        {
            auto body_result = interpret(*do_statement.body);
            if (body_result.is_error())
                return body_result;

            static_assert(static_cast<int>(InterpreterSignal::COUNT) == 3, "Update this code");
            if (body_result.has_signal())
            {
                if (body_result.get_signal() == InterpreterSignal::Break)
                    break;
                else if (body_result.get_signal() == InterpreterSignal::Continue)
                    continue;
            }

            result = eval(*do_statement.condition);
            if (result.is_error())
                return result;
            
        } while (std::get<Type::Primitive::Bool>(result.get_value().value));

        return InterpreterError::None;
    }

    InterpreterResult Interpreter::interpret(const binding::BoundFunctionStatement &function)
    {
        TRACE();
        // This function is NOT called by function call, but by declaration
        // only exists for tracing purposes, consistency and don't have to bother with skipping it in the interpreter
        return InterpreterError::None;
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
