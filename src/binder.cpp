#include "binding/binder.hpp"


#include "debug.hpp"

#include <utility>
#include <iostream>
#include <algorithm>


#include <string>
namespace scc
{
    std::optional<binding::BoundBinaryExpression::OperatorKind> Binder::operation_kind_from_string(const std::string op_kind) 
    {
        constexpr auto PLUS_ASSIGNMENT = "+=";
        constexpr auto MINUS_ASSIGNMENT = "-=";
        constexpr auto MULTIPLICATION_ASSIGNMENT = "*=";
        constexpr auto DIVISION_ASSIGNMENT = "/=";
        constexpr auto MODULO_ASSIGNMENT = "%=";
        constexpr auto BITWISE_AND_ASSIGNMENT = "&=";
        constexpr auto BITWISE_OR_ASSIGNMENT = "|=";
        constexpr auto BITWISE_XOR_ASSIGNMENT = "^=";
        constexpr auto BITWISE_SHIFT_LEFT_ASSIGNMENT = "<<=";
        constexpr auto BITWISE_SHIFT_RIGHT_ASSIGNMENT = ">>=";

        if (op_kind == PLUS_ASSIGNMENT)
            return binding::BoundBinaryExpression::OperatorKind::Addition;
        else if (op_kind == MINUS_ASSIGNMENT)
            return binding::BoundBinaryExpression::OperatorKind::Subtraction;
        else if (op_kind == MULTIPLICATION_ASSIGNMENT)
            return binding::BoundBinaryExpression::OperatorKind::Multiplication;
        else if (op_kind == DIVISION_ASSIGNMENT)
            return binding::BoundBinaryExpression::OperatorKind::Division;
        else if (op_kind == MODULO_ASSIGNMENT)
            return binding::BoundBinaryExpression::OperatorKind::Modulo;
        else if (op_kind == BITWISE_AND_ASSIGNMENT)
            return binding::BoundBinaryExpression::OperatorKind::BitwiseAnd;
        else if (op_kind == BITWISE_OR_ASSIGNMENT)
            return binding::BoundBinaryExpression::OperatorKind::BitwiseOr;
        else if (op_kind == BITWISE_XOR_ASSIGNMENT)
            return binding::BoundBinaryExpression::OperatorKind::BitwiseXor;
        else if (op_kind == BITWISE_SHIFT_LEFT_ASSIGNMENT)
            return binding::BoundBinaryExpression::OperatorKind::BitwiseShiftLeft;
        else if (op_kind == BITWISE_SHIFT_RIGHT_ASSIGNMENT)
            return binding::BoundBinaryExpression::OperatorKind::BitwiseShiftRight;

        return std::nullopt;
    }

    binding::BinderResult<binding::BoundNode> Binder::bind_impl(const TreeNode &node)
    {
        SCC_BINDER_RESULT_TYPE(bind_impl);
        switch (node.symbol())
        {
        case Parser::TRANSLATION_UNIT_SYMBOL:
            return bind_block_statement(node).add_location_to_value_if_ok(node.maybe_location());
        case Parser::EXPRESSION_STATEMENT_SYMBOL:
            return bind_expression_statement(node).add_location_to_value_if_ok(node.maybe_location());
        case Parser::COMPOUND_STATEMENT_SYMBOL:
            return bind_block_statement(node).add_location_to_value_if_ok(node.maybe_location());
        case Parser::DECLARATION_SYMBOL:
            return bind_declaration(node).add_location_to_value_if_ok(node.maybe_location());
        case Parser::IF_STATEMENT_SYMBOL:
            return bind_if_statement(node).add_location_to_value_if_ok(node.maybe_location());
        case Parser::WHILE_STATEMENT_SYMBOL:
            return bind_while_statement(node).add_location_to_value_if_ok(node.maybe_location());
        case Parser::DO_STATEMENT_SYMBOL:
            return bind_do_statement(node).add_location_to_value_if_ok(node.maybe_location());
        case Parser::BREAK_STATEMENT_SYMBOL:
            return bind_break_statement(node).add_location_to_value_if_ok(node.maybe_location());
        case Parser::CONTINUE_STATEMENT_SYMBOL:
            return bind_continue_statement(node).add_location_to_value_if_ok(node.maybe_location());
        case Parser::FUNCTION_DEFINITION_SYMBOL:
            return bind_function(node).add_location_to_value_if_ok(node.maybe_location());
        case Parser::RETURN_STATEMENT_SYMBOL:
            return bind_return_statement(node).add_location_to_value_if_ok(node.maybe_location());
        case Parser::FOR_STATEMENT_SYMBOL:
            return bind_for_statement(node).add_location_to_value_if_ok(node.maybe_location());
        case Parser::IDENTIFIER_SYMBOL:
        case Parser::NUMBER_LITERAL_SYMBOL:
        case Parser::ASSIGNMENT_EXPRESSION_SYMBOL:
            // bind_expression already adds location.. so no need to add it again
            return bind_expression(node); 
        case Parser::PREPROC_INCLUDE_SYMBOL:    // this should be handled in bind_block_statement
            return binding::BinderResult<ResultType>(binding::BinderError(ErrorKind::ReachedUnreachableError, node));
        case Parser::PREPROC_DEF_SYMBOL:       // this should be handled in bind_block_statement as well
            return binding::BinderResult<ResultType>(binding::BinderError(ErrorKind::ReachedUnreachableError, node));
        default:
            std::cerr << "Binder::bind_impl: Unhandled symbol: " << std::quoted(node.symbol_name()) << std::endl;
            return binding::BinderResult<ResultType>(binding::BinderError(ErrorKind::ReachedUnreachableError, node));
        }
    }

    binding::BinderResult<binding::BoundNode> Binder::bind(const TreeNode &node)
    {
        if (node.has_error())
            return binding::BinderResult<binding::BoundNode>(binding::BinderError(ErrorKind::ParserError, node));

        SCC_BINDER_RESULT_TYPE(bind);
        if (node.symbol() != Parser::TRANSLATION_UNIT_SYMBOL)
            return binding::BinderResult<ResultType>(binding::BinderError(ErrorKind::ReachedUnreachableError, node));

        return bind_impl(node);
    }
}