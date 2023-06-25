#pragma once

#include <string>
#include <fstream>
#include <sstream>

#include <tree_sitter/api.h>
#include <tree_sitter/parser.h>
#include <tree_sitter-c/parser-c.h>


namespace scc
{
    namespace debug
    {
        std::stringstream escape_string(const std::string &str);

        std::stringstream ast_as_json(const TSNode& node, const std::string &code);
        std::stringstream ast_as_puml(const TSNode& node, const std::string &code);

    } 
} 
