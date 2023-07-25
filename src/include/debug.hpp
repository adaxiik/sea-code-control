#pragma once

#include <string>
#include <fstream>
#include <sstream>

#include <tree_sitter/api.h>
#include <tree_sitter/parser.h>
#include <tree_sitter-c/parser-c.h>

#include "parser.hpp"
#include "treenode.hpp"
#include "binding/bound_node.hpp"

namespace scc
{
    namespace debug
    {
        void escape_string(std::ostream &ss, const std::string &str);

        void ast_as_json(std::ostream &ss, const ParserResult &parser_result);
        void ast_as_puml(std::ostream &ss, const ParserResult &parser_result);
        void ast_as_text_tree(std::ostream &ss, const ParserResult &parser_result);
        void bound_ast_as_text_tree(std::ostream &ss, const binding::BoundNode &bound_node);

        // void variable_as_string(std::ostream &ss, const scc::vm::Variable& var);
        // void vars_as_json(std::ostream &ss, const vm::VM& vm);

    } 
} 
