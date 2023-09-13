#pragma once
#include "parser.hpp"
#include "binding/binder.hpp"
#include "lowering/lowerer.hpp"
#include "interpreter_result.hpp"
#include "interpreter_state.hpp"
namespace scc
{
    constexpr auto STACK_SIZE = 4 * Memory::MEGABYTE;
    constexpr auto MAIN_FUNCTION_NAME = "main";
    class Interpreter
    {
    public:
        // Interpreter(): m_memory(0)
        //              , m_scope_stack(m_memory.allocate(STACK_SIZE) + STACK_SIZE) {} // TODO: 

        Interpreter()
        : m_state({ 
            Memory(0),
            InterpreterScopeStack(Memory(0).allocate(STACK_SIZE) + STACK_SIZE),
            {}
        }) {} 
        ~Interpreter() = default;
        ParserResult parse(const std::string &code);
        InterpreterResult interpret(const std::string &code);
        InterpreterResult interpret(const ParserResult &parse_result);

    private:
    
        Parser m_parser;
        Binder m_binder;
        Lowerer m_lowerer;
        InterpreterState m_state;
        

        InterpreterResult register_functions(binding::BoundBlockStatement &block_statement);

        // InterpreterResult interpret(const binding::BoundBlockStatement &block_statement);
        // InterpreterResult interpret(const binding::BoundStatement &statement);
        // InterpreterResult interpret(const binding::BoundVariableDeclarationStatement &variable_declaration);
        // InterpreterResult interpret(const binding::BoundVariableValueDeclarationStatement &variable_value_declaration);
        // InterpreterResult interpret(const binding::BoundVariablePointerDeclarationStatement &variable_pointer_declaration);
        // InterpreterResult interpret(const binding::BoundVariableStaticArrayDeclarationStatement &variable_reference_declaration);
        // InterpreterResult interpret(const binding::BoundIfStatement &if_statement);
        // InterpreterResult interpret(const binding::BoundWhileStatement &while_statement);
        // InterpreterResult interpret(const binding::BoundDoStatement &do_statement);
        // InterpreterResult interpret(const binding::BoundBreakStatement &break_statement);
        // InterpreterResult interpret(const binding::BoundContinueStatement &continue_statement);
        // InterpreterResult interpret(const binding::BoundFunctionStatement &function);
        // InterpreterResult interpret(const binding::BoundReturnStatement &return_statement);

        // InterpreterResult eval(const binding::BoundExpression &expression);
        // InterpreterResult eval(const binding::BoundBinaryExpression &binary_expression);
        // InterpreterResult eval(const binding::BoundCastExpression &cast_expression);
        // InterpreterResult eval(const binding::BoundParenthesizedExpression &parenthesized_expression);
        // InterpreterResult eval(const binding::BoundIdentifierExpression &identifier_expression);
        // InterpreterResult eval(const binding::BoundAssignmentExpression &assignment_expression);
        // InterpreterResult eval(const binding::BoundLiteralExpression &literal_expression);
        // InterpreterResult eval(const binding::BoundCallExpression &call_expression);
        

    };

}