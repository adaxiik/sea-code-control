#pragma once

#include <tree_sitter/api.h>
#include <tree_sitter/parser.h>
#include <tree_sitter-c/parser-c.h>

#include <string>
#include <memory>

namespace scc
{
    class Parser
    {
    private:
        std::unique_ptr<TSParser, decltype(&ts_parser_delete)> m_parser;
        std::unique_ptr<TSTree, decltype(&ts_tree_delete)> m_tree;
        TSNode m_root_node;
        const std::string m_code;
        const TSLanguage *m_language;

    public:
        explicit Parser(const std::string &code);
        ~Parser() = default;

        TSNode get_root_node() const;
        bool has_error() const;
        const std::string &get_code() const;

        std::string get_symbol_name(TSSymbol symbol) const;
        std::string get_node_value(TSNode node) const;

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

        static constexpr auto ABSTRACT_ARRAY_DECLARATOR_SYMBOL = 190;
        static constexpr auto ABSTRACT_FUNCTION_DECLARATOR_SYMBOL = 186;
        static constexpr auto ABSTRACT_PARENTHESIZED_DECLARATOR_SYMBOL = 175;
        static constexpr auto ABSTRACT_POINTER_DECLARATOR_SYMBOL = 182;
        static constexpr auto ARGUMENT_LIST_SYMBOL = 237;
        static constexpr auto ARRAY_DECLARATOR_SYMBOL = 187;
        static constexpr auto ASSIGNMENT_EXPRESSION_SYMBOL = 225;
        static constexpr auto ATTRIBUTED_DECLARATOR_SYMBOL = 176;
        static constexpr auto ATTRIBUTED_STATEMENT_SYMBOL = 209;
        static constexpr auto ATTRIBUTE_DECLARATION_SYMBOL = 161;
        static constexpr auto ATTRIBUTE_SPECIFIER_SYMBOL = 159;
        static constexpr auto ATTRIBUTE_SYMBOL = 160;
        static constexpr auto BINARY_EXPRESSION_SYMBOL = 228;
        static constexpr auto BITFIELD_CLAUSE_SYMBOL = 204;
        static constexpr auto BREAK_STATEMENT_SYMBOL = 219;
        static constexpr auto CALL_EXPRESSION_SYMBOL = 236;
        static constexpr auto CASE_STATEMENT_SYMBOL = 214;
        static constexpr auto CAST_EXPRESSION_SYMBOL = 230;
        static constexpr auto CHAR_LITERAL_SYMBOL = 245;
        static constexpr auto COMMA_EXPRESSION_SYMBOL = 223;
        static constexpr auto COMMENT_SYMBOL = 131;
        static constexpr auto COMPOUND_LITERAL_EXPRESSION_SYMBOL = 239;
        static constexpr auto COMPOUND_STATEMENT_SYMBOL = 192;
        static constexpr auto CONCATENATED_STRING_SYMBOL = 246;
        static constexpr auto CONDITIONAL_EXPRESSION_SYMBOL = 224;
        static constexpr auto CONTINUE_STATEMENT_SYMBOL = 220;
        static constexpr auto DECLARATION_LIST_SYMBOL = 167;
        static constexpr auto DECLARATION_SYMBOL = 154;
        static constexpr auto DO_STATEMENT_SYMBOL = 216;
        static constexpr auto ENUMERATOR_LIST_SYMBOL = 198;
        static constexpr auto ENUMERATOR_SYMBOL = 205;
        static constexpr auto ENUM_SPECIFIER_SYMBOL = 197;
        static constexpr auto ESCAPE_SEQUENCE_SYMBOL = 126;
        static constexpr auto EXPRESSION_STATEMENT_SYMBOL = 211;
        static constexpr auto FALSE_SYMBOL = 129;
        static constexpr auto FIELD_DECLARATION_LIST_SYMBOL = 201;
        static constexpr auto FIELD_DECLARATION_SYMBOL = 203;
        static constexpr auto FIELD_DESIGNATOR_SYMBOL = 244;
        static constexpr auto FIELD_EXPRESSION_SYMBOL = 238;
        static constexpr auto FIELD_IDENTIFIER_SYMBOL = 273;
        static constexpr auto FOR_STATEMENT_SYMBOL = 217;
        static constexpr auto FUNCTION_DECLARATOR_SYMBOL = 183;
        static constexpr auto FUNCTION_DEFINITION_SYMBOL = 153;
        static constexpr auto GENERIC_EXPRESSION_SYMBOL = 234;
        static constexpr auto GOTO_STATEMENT_SYMBOL = 221;
        static constexpr auto IDENTIFIER_SYMBOL = 1;
        static constexpr auto IF_STATEMENT_SYMBOL = 212;
        static constexpr auto INITIALIZER_LIST_SYMBOL = 241;
        static constexpr auto INITIALIZER_PAIR_SYMBOL = 242;
        static constexpr auto INIT_DECLARATOR_SYMBOL = 191;
        static constexpr auto LABELED_STATEMENT_SYMBOL = 210;
        static constexpr auto LINKAGE_SPECIFICATION_SYMBOL = 158;
        static constexpr auto MACRO_TYPE_SPECIFIER_SYMBOL = 249;
        static constexpr auto MS_BASED_MODIFIER_SYMBOL = 163;
        static constexpr auto MS_CALL_MODIFIER_SYMBOL = 164;
        static constexpr auto MS_DECLSPEC_MODIFIER_SYMBOL = 162;
        static constexpr auto MS_POINTER_MODIFIER_SYMBOL = 166;
        static constexpr auto MS_RESTRICT_MODIFIER_SYMBOL = 54;
        static constexpr auto MS_SIGNED_PTR_MODIFIER_SYMBOL = 56;
        static constexpr auto MS_UNALIGNED_PTR_MODIFIER_SYMBOL = 165;
        static constexpr auto MS_UNSIGNED_PTR_MODIFIER_SYMBOL = 55;
        static constexpr auto NULL_SYMBOL = 130;
        static constexpr auto NUMBER_LITERAL_SYMBOL = 113;
        static constexpr auto OFFSETOF_EXPRESSION_SYMBOL = 233;
        static constexpr auto PARAMETER_DECLARATION_SYMBOL = 208;
        static constexpr auto PARAMETER_LIST_SYMBOL = 207;
        static constexpr auto PARENTHESIZED_DECLARATOR_SYMBOL = 172;
        static constexpr auto PARENTHESIZED_EXPRESSION_SYMBOL = 240;
        static constexpr auto POINTER_DECLARATOR_SYMBOL = 179;
        static constexpr auto POINTER_EXPRESSION_SYMBOL = 226;
        static constexpr auto PREPROC_ARG_SYMBOL = 16;
        static constexpr auto PREPROC_CALL_SYMBOL = 137;
        static constexpr auto PREPROC_DEFINED_SYMBOL = 148;
        static constexpr auto PREPROC_DEF_SYMBOL = 134;
        static constexpr auto PREPROC_DIRECTIVE_SYMBOL = 15;
        static constexpr auto PREPROC_ELIF_SYMBOL = 141;
        static constexpr auto PREPROC_ELSE_SYMBOL = 140;
        static constexpr auto PREPROC_FUNCTION_DEF_SYMBOL = 135;
        static constexpr auto PREPROC_IFDEF_SYMBOL = 139;
        static constexpr auto PREPROC_IF_SYMBOL = 138;
        static constexpr auto PREPROC_INCLUDE_SYMBOL = 133;
        static constexpr auto PREPROC_PARAMS_SYMBOL = 136;
        static constexpr auto PRIMITIVE_TYPE_SYMBOL = 78;
        static constexpr auto RETURN_STATEMENT_SYMBOL = 218;
        static constexpr auto SIZED_TYPE_SPECIFIER_SYMBOL = 196;
        static constexpr auto SIZEOF_EXPRESSION_SYMBOL = 232;
        static constexpr auto STATEMENT_IDENTIFIER_SYMBOL = 274;
        static constexpr auto STORAGE_CLASS_SPECIFIER_SYMBOL = 193;
        static constexpr auto STRING_LITERAL_SYMBOL = 247;
        static constexpr auto STRUCT_SPECIFIER_SYMBOL = 199;
        static constexpr auto SUBSCRIPT_DESIGNATOR_SYMBOL = 243;
        static constexpr auto SUBSCRIPT_EXPRESSION_SYMBOL = 235;
        static constexpr auto SWITCH_STATEMENT_SYMBOL = 213;
        static constexpr auto SYSTEM_LIB_STRING_SYMBOL = 127;
        static constexpr auto TRANSLATION_UNIT_SYMBOL = 132;
        static constexpr auto TRUE_SYMBOL = 128;
        static constexpr auto TYPE_DEFINITION_SYMBOL = 155;
        static constexpr auto TYPE_DESCRIPTOR_SYMBOL = 231;
        static constexpr auto TYPE_IDENTIFIER_SYMBOL = 275;
        static constexpr auto TYPE_QUALIFIER_SYMBOL = 194;
        static constexpr auto UNARY_EXPRESSION_SYMBOL = 227;
        static constexpr auto UNION_SPECIFIER_SYMBOL = 200;
        static constexpr auto UPDATE_EXPRESSION_SYMBOL = 229;
        static constexpr auto VARIADIC_PARAMETER_SYMBOL = 206;
        static constexpr auto WHILE_STATEMENT_SYMBOL = 215;
    };
}
