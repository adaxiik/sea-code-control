#pragma once

#ifdef BUILDING_WASM

#include <emscripten/bind.h>
#include "parser.hpp"
#include "debug.hpp"

namespace scc
{
    namespace api
    {
        // wasm-bindgen doesn't support std::stringstream
        std::string debug_ast_as_json(const Parser& parser) 
        {
            return debug::ast_as_json(parser).str();
        }

        std::string debug_ast_as_puml(const Parser& parser)
        {
            return debug::ast_as_puml(parser).str();
        }
    }
}

EMSCRIPTEN_BINDINGS(scc_api)
{
    emscripten::class_<scc::Parser>("Parser")
        .constructor<const std::string&>()
        .function("has_error", &scc::Parser::has_error);

    emscripten::function("debug_ast_as_json", &scc::api::debug_ast_as_json);
    emscripten::function("debug_ast_as_puml", &scc::api::debug_ast_as_puml);
}

#endif