#pragma once
#include <vector>
#include <optional>

#include "parser.hpp"
#include "instructions/instruction.hpp"
#include "compile_context.hpp"
namespace scc
{
    class Compiler
    {
    public:
        using Instructions = std::vector<Instruction *>;
        Compiler(const Parser &parser) : m_parser(parser) {}
        std::optional<Instructions> compile();

    private:
        const Parser &m_parser;
        Instructions m_instructions;
        CompileContext m_context;

        void compile_impl(TSNode node);
        void compile_translation_unit(TSNode node);
        void compile_declaration(TSNode node);
        void compile_init_declarator(TSNode node);
        void compile_number_literal(TSNode node);
        void compile_expression_statement(TSNode node);
        void compile_assignment_expression(TSNode node);
        void compile_binary_expression(TSNode node);

        std::optional<type::Type> binary_expression_type(TSNode node);

    };

}