#pragma once
#include "parser.hpp"
#include "binding/binder.hpp"
#include "interpreter_result.hpp"
#include "interpreter_scope.hpp"
#include "memory.hpp"
namespace scc
{
    constexpr auto STACK_SIZE = 4 * Memory::MEGABYTE;
    class Interpreter
    {
    public:
        Interpreter(): m_memory(0)
                     , m_scope_stack(m_memory.allocate(STACK_SIZE) + STACK_SIZE) {} // TODO: 
        ~Interpreter() = default;
        ParserResult parse(const std::string &code);
        InterpreterResult interpret(const std::string &code);
        InterpreterResult interpret(const ParserResult &parse_result);

    private:
        template <typename T, typename U>
        const T& ikwid_rc (const U& u) // I know what I'm doing reference cast (used for downcasting)
        {
            static_assert(std::is_base_of_v<U,T>);
            return *static_cast<const T*>(&u);
        }

        Parser m_parser;
        Binder m_binder;
        Memory m_memory;
        InterpreterScopeStack m_scope_stack;

        InterpreterResult interpret(const binding::BoundBlockStatement &block_statement);
        InterpreterResult interpret(const binding::BoundStatement &statement);
        InterpreterResult interpret(const binding::BoundVariableDeclarationStatement &variable_declaration);
        InterpreterResult interpret(const binding::BoundVariableValueDeclarationStatement &variable_value_declaration);
        InterpreterResult interpret(const binding::BoundVariablePointerDeclarationStatement &variable_pointer_declaration);
        InterpreterResult interpret(const binding::BoundVariableStaticArrayDeclarationStatement &variable_reference_declaration);
        InterpreterResult interpret(const binding::BoundIfStatement &if_statement);
        InterpreterResult interpret(const binding::BoundWhileStatement &while_statement);
        InterpreterResult interpret(const binding::BoundDoStatement &do_statement);
        InterpreterResult interpret(const binding::BoundBreakStatement &break_statement);
        InterpreterResult interpret(const binding::BoundContinueStatement &continue_statement);
        InterpreterResult interpret(const binding::BoundFunctionStatement &function);

        InterpreterResult eval(const binding::BoundExpression &expression);
        InterpreterResult eval(const binding::BoundBinaryExpression &binary_expression);
        InterpreterResult eval(const binding::BoundCastExpression &cast_expression);
        InterpreterResult eval(const binding::BoundParenthesizedExpression &parenthesized_expression);
        InterpreterResult eval(const binding::BoundIdentifierExpression &identifier_expression);
        InterpreterResult eval(const binding::BoundAssignmentExpression &assignment_expression);
        InterpreterResult eval(const binding::BoundLiteralExpression &literal_expression);
        

    };

}