#pragma once

#include <tree_sitter/api.h>
#include <tree_sitter/parser.h>
#include <tree_sitter-c/parser-c.h>

#include <string>
#include <memory>

#include "treenode.hpp"

namespace scc
{
    class Parser;
    class ParserResult
    {
    public:
        using TSTreePtr = std::unique_ptr<TSTree, decltype(&ts_tree_delete)>;
    private:
        std::string m_code;
        TSTreePtr m_tree;
        const Parser& m_parser;
        bool m_want_to_remove_location;

    public:
        ParserResult(const std::string &code, TSTreePtr tree, const Parser& parser, bool want_to_remove_location);
        ParserResult(const ParserResult&);
        ~ParserResult() = default;

        TreeNode root_node() const;
        bool has_error() const;
        const std::string &code() const;
        const Parser& parser() const;
        bool want_to_remove_location() const;
    };


    class Parser
    {
    private:
        std::unique_ptr<TSParser, decltype(&ts_parser_delete)> m_parser;
        const TSLanguage *m_language;

    public:
        Parser();
        ~Parser() = default;
        std::string get_symbol_name(TSSymbol symbol) const;

        ParserResult parse(const std::string &code, bool want_to_remove_location = false) const;



        // =======================
        // = Tree-sitter symbols =
        // =======================

        // size_t symbol_count = ts_language_symbol_count(language);
        // std::cout << "symbol count: " << symbol_count << std::endl;
        // std::set<std::string> names;
        // for (size_t i = 0; i < symbol_count; i++)
        // {
        //     const char* name = ts_language_symbol_name(language, i);
        //     TSSymbolType type = ts_language_symbol_type(language, i);
        //     if(type == TSSymbolTypeRegular)
        //     {
        //         // some symbol names are there multiple times, with different ids.. I dunno why
        //         auto s = ts_language_symbol_for_name(language, name, std::strlen(name), true);
        //         std::string name_upper{name};
        //         std::transform(name_upper.begin(), name_upper.end(), name_upper.begin(), [](unsigned char c){ return std::toupper(c); });
        //         std::string result = "static constexpr auto " + name_upper + "_SYMBOL = " + std::to_string(s) + ";";
        //         names.insert(result);
        //     }
        // }

        // for(auto& name : names)
        //     std::cout << name << std::endl;



        // =======================
        // = Tree-sitter fields =
        // =======================

        // size_t field_count = ts_language_field_count(language);
        // std::cout << "field count: " << field_count << std::endl;
        // std::set<std::string> names;
        // for (size_t i = 0; i < field_count; i++)
        // {
        //     const char* name = ts_language_field_name_for_id(language, i);
        //     if (name == nullptr)
        //         continue;

        //     auto s = ts_language_field_id_for_name(language, name, std::strlen(name));
        //     std::string name_upper{name};
        //     std::transform(name_upper.begin(), name_upper.end(), name_upper.begin(), [](unsigned char c){ return std::toupper(c); });
        //     std::string result = "static constexpr auto " + name_upper + "_FIELD = " + std::to_string(s) + ";";
        //     names.insert(result);

        // }

        // for(auto& name : names)
        //     std::cout << name << std::endl;


        static constexpr auto ABSTRACT_ARRAY_DECLARATOR_SYMBOL = 177;
        static constexpr auto ABSTRACT_FUNCTION_DECLARATOR_SYMBOL = 173;
        static constexpr auto ABSTRACT_PARENTHESIZED_DECLARATOR_SYMBOL = 165;
        static constexpr auto ABSTRACT_POINTER_DECLARATOR_SYMBOL = 169;
        static constexpr auto ARGUMENT_LIST_SYMBOL = 220;
        static constexpr auto ARRAY_DECLARATOR_SYMBOL = 174;
        static constexpr auto ASSIGNMENT_EXPRESSION_SYMBOL = 210;
        static constexpr auto ATTRIBUTE_SPECIFIER_SYMBOL = 151;
        static constexpr auto BINARY_EXPRESSION_SYMBOL = 213;
        static constexpr auto BITFIELD_CLAUSE_SYMBOL = 191;
        static constexpr auto BREAK_STATEMENT_SYMBOL = 204;
        static constexpr auto CALL_EXPRESSION_SYMBOL = 219;
        static constexpr auto CASE_STATEMENT_SYMBOL = 199;
        static constexpr auto CAST_EXPRESSION_SYMBOL = 215;
        static constexpr auto CHAR_LITERAL_SYMBOL = 228;
        static constexpr auto COMMA_EXPRESSION_SYMBOL = 208;
        static constexpr auto COMMENT_SYMBOL = 124;
        static constexpr auto COMPOUND_LITERAL_EXPRESSION_SYMBOL = 222;
        static constexpr auto COMPOUND_STATEMENT_SYMBOL = 179;
        static constexpr auto CONCATENATED_STRING_SYMBOL = 229;
        static constexpr auto CONDITIONAL_EXPRESSION_SYMBOL = 209;
        static constexpr auto CONTINUE_STATEMENT_SYMBOL = 205;
        static constexpr auto DECLARATION_LIST_SYMBOL = 157;
        static constexpr auto DECLARATION_SYMBOL = 147;
        static constexpr auto DO_STATEMENT_SYMBOL = 201;
        static constexpr auto ENUMERATOR_LIST_SYMBOL = 185;
        static constexpr auto ENUMERATOR_SYMBOL = 192;
        static constexpr auto ENUM_SPECIFIER_SYMBOL = 184;
        static constexpr auto ESCAPE_SEQUENCE_SYMBOL = 119;
        static constexpr auto EXPRESSION_STATEMENT_SYMBOL = 196;
        static constexpr auto FALSE_SYMBOL = 122;
        static constexpr auto FIELD_DECLARATION_LIST_SYMBOL = 188;
        static constexpr auto FIELD_DECLARATION_SYMBOL = 190;
        static constexpr auto FIELD_DESIGNATOR_SYMBOL = 227;
        static constexpr auto FIELD_EXPRESSION_SYMBOL = 221;
        static constexpr auto FIELD_IDENTIFIER_SYMBOL = 253;
        static constexpr auto FOR_STATEMENT_SYMBOL = 202;
        static constexpr auto FUNCTION_DECLARATOR_SYMBOL = 170;
        static constexpr auto FUNCTION_DEFINITION_SYMBOL = 146;
        static constexpr auto GOTO_STATEMENT_SYMBOL = 206;
        static constexpr auto IDENTIFIER_SYMBOL = 1;
        static constexpr auto IF_STATEMENT_SYMBOL = 197;
        static constexpr auto INITIALIZER_LIST_SYMBOL = 224;
        static constexpr auto INITIALIZER_PAIR_SYMBOL = 225;
        static constexpr auto INIT_DECLARATOR_SYMBOL = 178;
        static constexpr auto LABELED_STATEMENT_SYMBOL = 195;
        static constexpr auto LINKAGE_SPECIFICATION_SYMBOL = 150;
        static constexpr auto MACRO_TYPE_SPECIFIER_SYMBOL = 232;
        static constexpr auto MS_BASED_MODIFIER_SYMBOL = 153;
        static constexpr auto MS_CALL_MODIFIER_SYMBOL = 154;
        static constexpr auto MS_DECLSPEC_MODIFIER_SYMBOL = 152;
        static constexpr auto MS_POINTER_MODIFIER_SYMBOL = 156;
        static constexpr auto MS_RESTRICT_MODIFIER_SYMBOL = 51;
        static constexpr auto MS_SIGNED_PTR_MODIFIER_SYMBOL = 53;
        static constexpr auto MS_UNALIGNED_PTR_MODIFIER_SYMBOL = 155;
        static constexpr auto MS_UNSIGNED_PTR_MODIFIER_SYMBOL = 52;
        static constexpr auto NULL_SYMBOL = 123;
        static constexpr auto NUMBER_LITERAL_SYMBOL = 106;
        static constexpr auto PARAMETER_DECLARATION_SYMBOL = 194;
        static constexpr auto PARAMETER_LIST_SYMBOL = 193;
        static constexpr auto PARENTHESIZED_DECLARATOR_SYMBOL = 162;
        static constexpr auto PARENTHESIZED_EXPRESSION_SYMBOL = 223;
        static constexpr auto POINTER_DECLARATOR_SYMBOL = 166;
        static constexpr auto POINTER_EXPRESSION_SYMBOL = 211;
        static constexpr auto PREPROC_ARG_SYMBOL = 16;
        static constexpr auto PREPROC_CALL_SYMBOL = 130;
        static constexpr auto PREPROC_DEFINED_SYMBOL = 141;
        static constexpr auto PREPROC_DEF_SYMBOL = 127;
        static constexpr auto PREPROC_DIRECTIVE_SYMBOL = 15;
        static constexpr auto PREPROC_ELIF_SYMBOL = 134;
        static constexpr auto PREPROC_ELSE_SYMBOL = 133;
        static constexpr auto PREPROC_FUNCTION_DEF_SYMBOL = 128;
        static constexpr auto PREPROC_IFDEF_SYMBOL = 132;
        static constexpr auto PREPROC_IF_SYMBOL = 131;
        static constexpr auto PREPROC_INCLUDE_SYMBOL = 126;
        static constexpr auto PREPROC_PARAMS_SYMBOL = 129;
        static constexpr auto PRIMITIVE_TYPE_SYMBOL = 73;
        static constexpr auto RETURN_STATEMENT_SYMBOL = 203;
        static constexpr auto SIZED_TYPE_SPECIFIER_SYMBOL = 183;
        static constexpr auto SIZEOF_EXPRESSION_SYMBOL = 217;
        static constexpr auto STATEMENT_IDENTIFIER_SYMBOL = 254;
        static constexpr auto STORAGE_CLASS_SPECIFIER_SYMBOL = 180;
        static constexpr auto STRING_LITERAL_SYMBOL = 230;
        static constexpr auto STRUCT_SPECIFIER_SYMBOL = 186;
        static constexpr auto SUBSCRIPT_DESIGNATOR_SYMBOL = 226;
        static constexpr auto SUBSCRIPT_EXPRESSION_SYMBOL = 218;
        static constexpr auto SWITCH_STATEMENT_SYMBOL = 198;
        static constexpr auto SYSTEM_LIB_STRING_SYMBOL = 120;
        static constexpr auto TRANSLATION_UNIT_SYMBOL = 125;
        static constexpr auto TRUE_SYMBOL = 121;
        static constexpr auto TYPE_DEFINITION_SYMBOL = 148;
        static constexpr auto TYPE_DESCRIPTOR_SYMBOL = 216;
        static constexpr auto TYPE_IDENTIFIER_SYMBOL = 255;
        static constexpr auto TYPE_QUALIFIER_SYMBOL = 181;
        static constexpr auto UNARY_EXPRESSION_SYMBOL = 212;
        static constexpr auto UNION_SPECIFIER_SYMBOL = 187;
        static constexpr auto UPDATE_EXPRESSION_SYMBOL = 214;
        static constexpr auto WHILE_STATEMENT_SYMBOL = 200;

        static constexpr auto ALTERNATIVE_FIELD = 1;
        static constexpr auto ARGUMENTS_FIELD = 3;
        static constexpr auto ARGUMENT_FIELD = 2;
        static constexpr auto BODY_FIELD = 4;
        static constexpr auto CONDITION_FIELD = 5;
        static constexpr auto CONSEQUENCE_FIELD = 6;
        static constexpr auto DECLARATOR_FIELD = 7;
        static constexpr auto DESIGNATOR_FIELD = 8;
        static constexpr auto DIRECTIVE_FIELD = 9;
        static constexpr auto FIELD_FIELD = 10;
        static constexpr auto FUNCTION_FIELD = 11;
        static constexpr auto INDEX_FIELD = 12;
        static constexpr auto INITIALIZER_FIELD = 13;
        static constexpr auto LABEL_FIELD = 14;
        static constexpr auto LEFT_FIELD = 15;
        static constexpr auto NAME_FIELD = 16;
        static constexpr auto OPERATOR_FIELD = 17;
        static constexpr auto PARAMETERS_FIELD = 18;
        static constexpr auto PATH_FIELD = 19;
        static constexpr auto RIGHT_FIELD = 20;
        static constexpr auto SIZE_FIELD = 21;
        static constexpr auto TYPE_FIELD = 22;
        static constexpr auto UPDATE_FIELD = 23;
    };
}
