#pragma once

#ifdef BUILDING_WASM

#include <emscripten/bind.h>
#include <emscripten.h>

#include "debug.hpp"
#include "interpreter.hpp"
#include "parser.hpp"
#include "interpreter_io.hpp"
#include "export.hpp"

namespace scc
{
    namespace api
    {
        // wasm-bindgen doesn't support std::stringstream
        std::string debug_ast_as_json(const ParserResult& parser_result)
        {
            std::stringstream ss;
            debug::ast_as_json(ss, parser_result);
            return ss.str();
        }

        std::string debug_ast_as_puml(const ParserResult& parser_result)
        {
            std::stringstream ss;
            debug::ast_as_puml(ss, parser_result);
            return ss.str();
        }

        std::string debug_interpreter_error_as_text(InterpreterError error)
        {
            std::stringstream ss;
            debug::interpreter_error_as_text(ss, error);
            return ss.str();
        }

        void set_stdout_callback(const std::string& fn_name)
        {
            scc::InterpreterIO::set_stdout_callback([fn_name](const char* str) {
                EM_ASM({
                    var fn_name = UTF8ToString($0);
                    var str = UTF8ToString($1);
                    window[fn_name](str);
                }, fn_name.c_str(), str);
            });
        }

        void set_stderr_callback(const std::string& fn_name)
        {
            scc::InterpreterIO::set_stderr_callback([fn_name](const char* str) {
                EM_ASM({
                    var fn_name = UTF8ToString($0);
                    var str = UTF8ToString($1);
                    window[fn_name](str);
                }, fn_name.c_str(), str);
            });
        }

        binding::BoundBlockStatement* cast_bound_node_to_bound_block_statement(binding::BoundNode* node)
        {
            return static_cast<binding::BoundBlockStatement*>(node);
        }

        void debug_print_location(Location location)
        {
            std::cout << location.row << ":" << location.column << std::endl;
        }

        uint32_t get_location_row(Location location)
        {
            return location.row;
        }

        std::string snapshot_running_interpreter_as_json(const RunningInterpreter* running_interpreter)
        {
            return export_format::to_json(export_format::make_snapshot(running_interpreter->state()));
        }
    }
}

EMSCRIPTEN_BINDINGS(scc_api)
{
    // emscripten::class_<scc::Parser>("Parser")
    //     .constructor<const std::string&>()
    //     .function("has_error", &scc::Parser::has_error);
    emscripten::class_<scc::TreeNode>("TreeNode");

    emscripten::function("cast_bound_node_to_bound_block_statement", &scc::api::cast_bound_node_to_bound_block_statement, emscripten::allow_raw_pointers());

    emscripten::class_<scc::ParserResult>("ParserResult")
        .function("has_error", &scc::ParserResult::has_error)
        .function("root_node", &scc::ParserResult::root_node);

    emscripten::class_<scc::InterpreterResult>("InterpreterResult")
        .function("is_ok", &scc::InterpreterResult::is_ok)
        .function("is_error", &scc::InterpreterResult::is_error)
        .function("is_ok_and_has_value", &scc::InterpreterResult::is_ok_and_has_value)
        .function("get_error", &scc::InterpreterResult::get_error);

    emscripten::enum_<scc::InterpreterError>("InterpreterError")
        .value("None", scc::InterpreterError::None)
        .value("ParseError", scc::InterpreterError::ParseError)
        .value("BindError", scc::InterpreterError::BindError)
        .value("RuntimeError", scc::InterpreterError::RuntimeError)
        .value("ReachedUnreachableError", scc::InterpreterError::ReachedUnreachableError)
        .value("InvalidOperationError", scc::InterpreterError::InvalidOperationError)
        .value("DivisionByZeroError", scc::InterpreterError::DivisionByZeroError)
        .value("VariableAlreadyExistsError", scc::InterpreterError::VariableAlreadyExistsError)
        .value("VariableDoesntExistError", scc::InterpreterError::VariableDoesntExistError)
        .value("VariableNotInitializedError", scc::InterpreterError::VariableNotInitializedError)
        .value("UnhandeledSignalError", scc::InterpreterError::UnhandeledSignalError)
        .value("MissingMainFunctionError", scc::InterpreterError::MissingMainFunctionError)
        .value("FunctionAlreadyDefinedError", scc::InterpreterError::FunctionAlreadyDefinedError)
        .value("FunctionNotDefinedError", scc::InterpreterError::FunctionNotDefinedError)
        .value("FunctionNotDeclaredError", scc::InterpreterError::FunctionNotDeclaredError)
        .value("FunctionArgumentCountMismatchError", scc::InterpreterError::FunctionArgumentCountMismatchError)
        .value("IncosistentFunctionSignatureError", scc::InterpreterError::IncosistentFunctionSignatureError)
        .value("MissingReturnStatementError", scc::InterpreterError::MissingReturnStatementError)
        .value("MissingReturnValueError", scc::InterpreterError::MissingReturnValueError)
        .value("BreakpointReachedError", scc::InterpreterError::BreakpointReachedError)
        .value("AssertionFailedError", scc::InterpreterError::AssertionFailedError)
        .value("MemoryError", scc::InterpreterError::MemoryError)
        .value("DereferenceError", scc::InterpreterError::DereferenceError)
        .value("NotEnoughValuesToDropError", scc::InterpreterError::NotEnoughValuesToDropError)
        .value("COUNT", scc::InterpreterError::COUNT);

    static_assert(static_cast<int>(scc::InterpreterError::COUNT) == 24);



    // emscripten::class_<scc::Interpreter>("Interpreter")
    //     .constructor<>()
    //     .function("parse", &scc::Interpreter::parse, emscripten::allow_raw_pointers())
    //     .function("interpret_string", emscripten::select_overload<scc::InterpreterResult(const std::string&)>(&scc::Interpreter::interpret), emscripten::allow_raw_pointers())
    //     .function("interpret_parserresult", emscripten::select_overload<scc::InterpreterResult(const scc::ParserResult&)>(&scc::Interpreter::interpret), emscripten::allow_raw_pointers());

    emscripten::function("debug_ast_as_json", &scc::api::debug_ast_as_json);
    emscripten::function("debug_ast_as_puml", &scc::api::debug_ast_as_puml);
    emscripten::function("debug_interpreter_error_as_text", &scc::api::debug_interpreter_error_as_text);
    emscripten::function("debug_print_location", &scc::api::debug_print_location);

    emscripten::function("get_location_row", &scc::api::get_location_row);

    emscripten::function("set_stdout_callback", &scc::api::set_stdout_callback, emscripten::allow_raw_pointers());
    emscripten::function("set_stderr_callback", &scc::api::set_stderr_callback, emscripten::allow_raw_pointers());

    emscripten::class_<scc::InterpreterState>("InterpreterState");

    emscripten::class_<scc::RunningInterpreter>("RunningInterpreter")
        .function("continue_execution", &scc::RunningInterpreter::continue_execution, emscripten::allow_raw_pointers())
        .function("next", &scc::RunningInterpreter::next, emscripten::allow_raw_pointers())
        .function("breakpoints", &scc::RunningInterpreter::breakpoints, emscripten::allow_raw_pointers())
        .function("current_location", &scc::RunningInterpreter::current_location, emscripten::allow_raw_pointers());

    emscripten::class_<scc::Breakpoints>("Breakpoints")
        .function("add", &scc::Breakpoints::add, emscripten::allow_raw_pointers())
        .function("remove", &scc::Breakpoints::remove, emscripten::allow_raw_pointers())
        .function("contains", &scc::Breakpoints::contains)
        .function("clear", &scc::Breakpoints::clear);

    emscripten::class_<scc::binding::BinderError>("BinderError");
    emscripten::class_<scc::binding::BinderResult<scc::binding::BoundNode>>("BinderResult")
        .function("get_value", &scc::binding::BinderResult<scc::binding::BoundNode>::get_value, emscripten::allow_raw_pointers());

    emscripten::class_<scc::binding::BoundBlockStatement>("BoundBlockStatement");
    emscripten::class_<scc::binding::BoundNode>("BoundNode");

    emscripten::class_<std::vector<std::pair<scc::lowering::Instruction, std::optional<scc::Location>>>>("LocationAnotatedProgram")
        .function("size", &std::vector<std::pair<scc::lowering::Instruction, std::optional<scc::Location>>>::size);

    emscripten::class_<scc::Location>("Location");

    emscripten::class_<scc::Interpreter::RunningInterpreterCreationResult>("RunningInterpreterCreationResult")
        .function("is_error", &scc::Interpreter::RunningInterpreterCreationResult::is_error)
        .function("value", &scc::Interpreter::RunningInterpreterCreationResult::value, emscripten::allow_raw_pointers());

    emscripten::class_<scc::Interpreter>("Interpreter")
        .constructor<>()
        .function("parse", &scc::Interpreter::parse, emscripten::allow_raw_pointers())
        .function("bind", &scc::Interpreter::bind, emscripten::allow_raw_pointers())
        .function("lower", &scc::Interpreter::lower, emscripten::allow_raw_pointers())
        .function("compile_or_nothing", &scc::Interpreter::compile_or_nothing, emscripten::allow_raw_pointers())
        .function("interpret_string", emscripten::select_overload<scc::Interpreter::RunningInterpreterCreationResult(const std::string&)>(&scc::Interpreter::interpret), emscripten::allow_raw_pointers())
        .function("interpret_parserresult", emscripten::select_overload<scc::Interpreter::RunningInterpreterCreationResult(const scc::ParserResult&)>(&scc::Interpreter::interpret), emscripten::allow_raw_pointers())
        .function("interpret_binderresult", emscripten::select_overload<scc::Interpreter::RunningInterpreterCreationResult(const scc::binding::BinderResult<scc::binding::BoundNode>&)>(&scc::Interpreter::interpret), emscripten::allow_raw_pointers())
        .function("interpret_locationannotatedprogram", emscripten::select_overload<scc::Interpreter::RunningInterpreterCreationResult(const std::vector<std::pair<scc::lowering::Instruction, std::optional<scc::Location>>>&)>(&scc::Interpreter::interpret), emscripten::allow_raw_pointers());

        // I spend 4 hours debugging this line with no success. I have no idea why it doesn't work.. fucking line
        // .function("interpret", emscripten::select_overload<scc::RunningInterpreterCreationResult(void)>, emscripten::allow_raw_pointers());


    // emscripten::function("snapshot_to_json", emscripten::select_overload<std::string(const scc::export_format::ProgramSnapshot&)>(&scc::export_format::to_json),  emscripten::allow_raw_pointers());

    emscripten::function("snapshot_running_interpreter_as_json", &scc::api::snapshot_running_interpreter_as_json, emscripten::allow_raw_pointers());

}
#endif