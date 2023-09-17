#pragma once
#include "parser.hpp"
#include "binding/binder.hpp"
#include "lowering/lowerer.hpp"
#include "interpreter_result.hpp"
#include "interpreter_state.hpp"
namespace scc
{
    constexpr auto STACK_SIZE = 4 * Memory::MEGABYTE;
    constexpr auto GLOBAL_SCOPE_SIZE = 1 * Memory::MEGABYTE;
    constexpr auto MAIN_FUNCTION_NAME = "main";
    class Interpreter
    {
    public:
        Interpreter(): m_state(InterpreterState(0, STACK_SIZE, GLOBAL_SCOPE_SIZE)){}
        ~Interpreter() = default;
        ParserResult parse(const std::string &code);
        InterpreterResult interpret(const std::string &code);
        InterpreterResult interpret(const ParserResult &parse_result);

    private:
    
        Parser m_parser;
        Binder m_binder;
        Lowerer m_lowerer;
        InterpreterState m_state;
        std::vector<lowering::Instruction> m_program;
        
        InterpreterResult register_functions(binding::BoundBlockStatement &block_statement);

    };

}