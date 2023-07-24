#pragma once

#include <string>
#include <fstream>
#include <sstream>

#include <tree_sitter/api.h>
#include <tree_sitter/parser.h>
#include <tree_sitter-c/parser-c.h>

#include "parser.hpp"
#include "treenode.hpp"

namespace scc
{
    namespace debug
    {
        void escape_string(std::ostream &ss, const std::string &str);

        void ast_as_json(std::ostream &ss, const ParserResult &parser_result);
        void ast_as_puml(std::ostream &ss, const ParserResult &parser_result);
        void ast_as_text_tree(std::ostream &ss, const ParserResult &parser_result);

        // void variable_as_string(std::ostream &ss, const scc::vm::Variable& var);
        // void vars_as_json(std::ostream &ss, const vm::VM& vm);

    } 
} 
