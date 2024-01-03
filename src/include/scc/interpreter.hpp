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

        InterpreterResult continue_execution();
        InterpreterResult next();

        #ifndef BUILDING_WASM
        Breakpoints& breakpoints() { return m_breakpoints; }
        #else
        // WTF, whyyy.. It took me a while to figure out
        Breakpoints* breakpoints() { return &m_breakpoints; }
        #endif

        const InterpreterState& state() const { return m_state; }

    private:  
        InterpreterState m_state;
        Location m_current_location;
        LocationAnotatedProgram m_program;
        Breakpoints m_breakpoints;
        bool m_continuing_after_breakpoint;

        InterpreterResult execute_in_loop();
    };

    

    class Interpreter
    {
    public:
        struct RunningInterpreterCreationResult
        {
            std::optional<RunningInterpreter> running_interpreter;
            std::optional<binding::BinderError> binder_error;
            bool is_error() const { return binder_error.has_value(); }
            RunningInterpreter value() { return std::move(running_interpreter.value()); }
        };

        ParserResult parse(const std::string &code);
        binding::BinderResult<binding::BoundNode> bind(const TreeNode& root_node);
        LocationAnotatedProgram lower(const binding::BoundBlockStatement* bound_block_statement);

        std::optional<LocationAnotatedProgram> compile_or_nothing(const std::string& code);

        RunningInterpreterCreationResult interpret(const std::string &code);
        RunningInterpreterCreationResult interpret(const ParserResult &parse_result);
        RunningInterpreterCreationResult interpret(const binding::BinderResult<binding::BoundNode>& bind_result);
        RunningInterpreterCreationResult interpret(const LocationAnotatedProgram& program);
        RunningInterpreterCreationResult interpret();

    private:
        Parser m_parser;
        Binder m_binder;
        Lowerer m_lowerer;
    };


}