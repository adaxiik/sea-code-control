#pragma once

#ifdef BUILDING_WASM

#include <emscripten/bind.h>
#include "debug.hpp"
#include "interpreter.hpp"
#include "parser.hpp"

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
    }
}

EMSCRIPTEN_BINDINGS(scc_api)
{
    // emscripten::class_<scc::Parser>("Parser")
    //     .constructor<const std::string&>()
    //     .function("has_error", &scc::Parser::has_error);


    emscripten::class_<scc::ParserResult>("ParserResult")
        .function("has_error", &scc::ParserResult::has_error);


    emscripten::class_<scc::Interpreter>("Interpreter")
        .constructor<>()
        .function("parse", &scc::Interpreter::parse, emscripten::allow_raw_pointers())
        .function("interpret", static_cast<scc::InterpreterResult (scc::Interpreter::*)(const std::string&)>(&scc::Interpreter::interpret), emscripten::allow_raw_pointers())
        .function("interpret", static_cast<scc::InterpreterResult (scc::Interpreter::*)(const scc::ParserResult&)>(&scc::Interpreter::interpret), emscripten::allow_raw_pointers());

    emscripten::function("debug_ast_as_json", &scc::api::debug_ast_as_json);
    emscripten::function("debug_ast_as_puml", &scc::api::debug_ast_as_puml);
}

#endif