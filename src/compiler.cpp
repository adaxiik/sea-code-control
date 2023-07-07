#include "compiler.hpp"

#include <tree_sitter/api.h>
#include <tree_sitter/parser.h>
#include <tree_sitter-c/parser-c.h>

#include <iomanip>
#include <iostream>
#include <optional>

#include "type.hpp"
#include "instructions.hpp"

#define COMPILER_UNEXPECTED(node, message)                                                                                                                            \
    do                                                                                                                                                                \
    {                                                                                                                                                                 \
        std::cerr << "======================" << std::endl;                                                                                                           \
        std::cerr << "Unexpected node: " << std::quoted(m_parser.get_symbol_name(ts_node_symbol(node))) << " at line " << ts_node_start_point(node).row << std::endl; \
        std::cerr << (message) << std::endl;                                                                                                                          \
        std::cerr << __FILE__ << ":" << __LINE__ << std::endl;                                                                                                        \
        std::exit(1);                                                                                                                                                 \
    } while (0)

namespace scc
{
    std::optional<Compiler::Instructions> Compiler::compile()
    {
        if (m_parser.has_error())
            return std::nullopt;

        m_instructions.clear();
        compile_impl(m_parser.get_root_node());
        return m_instructions;
    }

    void Compiler::compile_translation_unit(TSNode node)
    {
        auto child_count = ts_node_named_child_count(node);
        for (size_t i = 0; i < child_count; i++)
        {
            auto child = ts_node_named_child(node, i);
            compile_impl(child);
        }
    }

    void Compiler::compile_declaration(TSNode node)
    {
        // type, [init, identifier]
        auto child_count = ts_node_named_child_count(node);
        if (child_count != 2 && child_count != 3)
        {
            COMPILER_UNEXPECTED(node, "Expected 2 or 3 children");
            return;
        }
        int id = 0;

        bool is_const = false;
        if (child_count == 3)
        {
            auto qualifier = ts_node_named_child(node, id++);
            if (ts_node_symbol(qualifier) != Parser::TYPE_QUALIFIER_SYMBOL)
                COMPILER_UNEXPECTED(qualifier, "Unexpected node");

            auto qualifier_text = m_parser.get_node_value(qualifier);
            if (qualifier_text == "const")
                is_const = true;
            else
                COMPILER_UNEXPECTED(qualifier, "Unexpected qualifier: " + qualifier_text);

        }

        auto decl_type = ts_node_named_child(node, id++);
        if (ts_node_symbol(decl_type) != Parser::PRIMITIVE_TYPE_SYMBOL) // TODO: add support for struct, sized_type_specifier..
            COMPILER_UNEXPECTED(decl_type, "Expected primitive type");
 

        auto type = type::Type::from_string(m_parser.get_node_value(decl_type));
        if (!type)
            COMPILER_UNEXPECTED(decl_type, "Unknown type " + m_parser.get_node_value(decl_type));
        
        type.value().is_const = is_const;
        auto identifier = ts_node_named_child(node, id++);
        auto symbol = ts_node_symbol(identifier);

        if (symbol == Parser::INIT_DECLARATOR_SYMBOL)
        {
            auto identifier_text_node = ts_node_named_child(identifier, 0);
            if (ts_node_symbol(identifier_text_node) != Parser::IDENTIFIER_SYMBOL)
                COMPILER_UNEXPECTED(identifier_text_node, "Unexpected node");
            
            auto identifier_text = m_parser.get_node_value(identifier_text_node);
            m_instructions.push_back(new instructions::NewVariable(identifier_text, type.value()));

            compile_init_declarator(identifier);
            return;
        }
        else if (symbol == Parser::IDENTIFIER_SYMBOL)
        {
            auto identifier_text = m_parser.get_node_value(identifier);
            m_instructions.push_back(new instructions::NewVariable(identifier_text, type.value()));
        }
        else
        {
            COMPILER_UNEXPECTED(identifier, "Unexpected node");
        }

    }

    void Compiler::compile_init_declarator(TSNode node)
    {
        auto child_count = ts_node_named_child_count(node);
        if (child_count != 2)
            COMPILER_UNEXPECTED(node, "Expected 2 children");

        auto identifier = ts_node_named_child(node,0);
        if (ts_node_symbol(identifier) != Parser::IDENTIFIER_SYMBOL)
            COMPILER_UNEXPECTED(identifier, "Expected identifier");
        



        auto initializer = ts_node_named_child(node,1);
        compile_impl(initializer);

        auto identifier_text = m_parser.get_node_value(identifier);
        m_instructions.push_back(new instructions::GetPtrToVar(identifier_text));
        m_instructions.push_back(new instructions::Store(4)); // TODOOOOOOOOOOOOOOOOOOOOOOOOOOOOO: support other sizes
    }

    void Compiler::compile_number_literal(TSNode node)
    {
        // TODOOOOOO: add support for floats somehow
        auto value = m_parser.get_node_value(node);
        m_instructions.push_back(new instructions::PushNumber(std::stoi(value)));
    }

    void Compiler::compile_expression_statement(TSNode node)
    {
        auto child_count = ts_node_named_child_count(node);
        if (child_count != 1)
            COMPILER_UNEXPECTED(node, "Expected 1 child");

        auto expression = ts_node_named_child(node, 0);
        compile_impl(expression);
    }

    void Compiler::compile_assignment_expression(TSNode node)
    {
        auto child_count = ts_node_named_child_count(node);
        if (child_count != 2)
            COMPILER_UNEXPECTED(node, "Expected 2 children");

        auto identifier = ts_node_named_child(node, 0);
        if (ts_node_symbol(identifier) != Parser::IDENTIFIER_SYMBOL)
            COMPILER_UNEXPECTED(identifier, "Expected identifier");




        compile_impl(ts_node_named_child(node, 1));
        auto identifier_text = m_parser.get_node_value(identifier);
        m_instructions.push_back(new instructions::GetPtrToVar(identifier_text));
        m_instructions.push_back(new instructions::Store(4)); // TODOOOOOOOOOOOOOOOOOOO: support other sizes
    }

    void Compiler::compile_binary_expression(TSNode node)
    {
        size_t child_count = ts_node_child_count(node); // NOTE: not named child count
        if (child_count != 3)
            COMPILER_UNEXPECTED(node, "Expected 3 children");
        
        compile_impl(ts_node_child(node, 0));
        compile_impl(ts_node_child(node, 2));

        auto operator_node = ts_node_child(node, 1);
        auto operator_text = m_parser.get_node_value(operator_node);

        if (operator_text == "+")
            m_instructions.push_back(new instructions::Add(type::Type(type::int_type{})));
        else
            COMPILER_UNEXPECTED(operator_node, "Operator \"" + operator_text + "\" is not implemented yet");
        
    }


    void Compiler::compile_impl(TSNode node)
    {
        if (ts_node_is_null(node))
            return;

        if (!ts_node_is_named(node))
            return;

        // if (ts_node_has_error(node)) we already checked for errors in the parser
        //     return;
        auto symbol = ts_node_symbol(node);
        switch (symbol)
        {
        case Parser::TRANSLATION_UNIT_SYMBOL:
            compile_translation_unit(node);
            break;
        case Parser::DECLARATION_SYMBOL:
            compile_declaration(node);
            break;
        case Parser::NUMBER_LITERAL_SYMBOL:
            compile_number_literal(node);
            break;
        case Parser::EXPRESSION_STATEMENT_SYMBOL:
            compile_expression_statement(node);
            break;
        case Parser::ASSIGNMENT_EXPRESSION_SYMBOL:
            compile_assignment_expression(node);
            break;
        case Parser::IDENTIFIER_SYMBOL:
            // if is just an identifier, it its loading a variable.. 
            // all other casess should are handled by the parent node.. might change in the future as compiler gets more mature
            m_instructions.push_back(new instructions::GetPtrToVar(m_parser.get_node_value(node)));
            m_instructions.push_back(new instructions::Load(4)); // TODOOOOOOOOOOOOOOOOOO: only 4bytes for now
            break;
        case Parser::BINARY_EXPRESSION_SYMBOL:
            compile_binary_expression(node);
            break;
        case Parser::COMMENT_SYMBOL:
            break;
        default:
            std::cerr << m_parser.get_symbol_name(symbol) << " is not implemented yet" << std::endl;
        }
    }
}