#pragma once
#include "parser.hpp"
#include "binding/binder.hpp"
#include "lowering/lowerer.hpp"
#include "interpreter_result.hpp"
#include "interpreter_state.hpp"
#include "breakpoints.hpp"
namespace scc
{
    constexpr auto STACK_SIZE = 4 * Memory::MEGABYTE;
    constexpr auto GLOBAL_SCOPE_SIZE = 1 * Memory::MEGABYTE;
    constexpr auto MAIN_FUNCTION_NAME = "main";
    // class Interpreter
    // {
    // public:
    //     Interpreter(): m_state(InterpreterState(0, STACK_SIZE, GLOBAL_SCOPE_SIZE)){}
    //     ~Interpreter() = default;
    //     ParserResult parse(const std::string &code);
    //     InterpreterResult interpret(const std::string &code);
    //     InterpreterResult interpret(const ParserResult &parse_result);
    //     Breakpoints& breakpoints();
    //     void continue_execution();
    //     void step_into();
    //     void step_over();
    // private:
    
    //     Parser m_parser;
    //     Binder m_binder;
    //     Lowerer m_lowerer;
    //     InterpreterState m_state;
    //     std::vector<std::pair<lowering::Instruction, std::optional<Location>>> m_program;
    //     Breakpoints m_breakpoints;
    // };

    using LocationAnotatedProgram = std::vector<std::pair<lowering::Instruction, std::optional<Location>>>;
    class RunningInterpreter
    {
    public:
        RunningInterpreter(const LocationAnotatedProgram& program);
        ~RunningInterpreter() = default;
        RunningInterpreter(RunningInterpreter&&) = default;
        RunningInterpreter& operator=(RunningInterpreter&&) = default;
        RunningInterpreter(const RunningInterpreter&) = delete;
        RunningInterpreter& operator=(const RunningInterpreter&) = delete;

        Location current_location() const;

        RunningInterpreter& append_code(const LocationAnotatedProgram& program);

        // InterpreterResult run();
        InterpreterResult continue_execution();
        InterpreterResult next();
        
        Breakpoints& breakpoints();
    private:  
        InterpreterState m_state;
        Location m_current_location;
        LocationAnotatedProgram m_program;
        Breakpoints m_breakpoints;
    };


    class Interpreter
    {
    public:
        struct RunningInterpreterCreationResult
        {
            std::optional<RunningInterpreter> running_interpreter;
            std::optional<binding::BinderError> binder_error;
            bool is_error() const { return binder_error.has_value(); }
            RunningInterpreter&& value() { return std::move(running_interpreter.value()); }
        };


        ParserResult parse(const std::string &code);
        binding::BinderResult<binding::BoundNode> bind(const TreeNode& root_node);
        LocationAnotatedProgram lower(const binding::BoundBlockStatement* bound_block_statement);

        std::optional<LocationAnotatedProgram> compile_or_nothing(const std::string& code);

        RunningInterpreterCreationResult interpret(const std::string &code);
        RunningInterpreterCreationResult interpret(const ParserResult &parse_result);
        RunningInterpreterCreationResult interpret(const binding::BinderResult<binding::BoundNode>& bind_result);
        RunningInterpreterCreationResult interpret();

    private:
        Parser m_parser;
        Binder m_binder;
        Lowerer m_lowerer;
    };


}