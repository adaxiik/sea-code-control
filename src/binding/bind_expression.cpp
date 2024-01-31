#include "binding/binder.hpp"

namespace scc
{
    binding::BinderResult<binding::BoundExpression> Binder::bind_expression(const TreeNode &node)
    {
        // okay, this function actually becomes redundat, because TreeSitter is so inconsistent
        // some functions call this directly so we are adding location here as well

        // SCC_UNIMPLEMENTED();
        switch (node.symbol())
        {
        case Parser::BINARY_EXPRESSION_SYMBOL:
            return bind_binary_expression(node).add_location_to_value_if_ok(node.maybe_location());
        case Parser::CAST_EXPRESSION_SYMBOL:
            return bind_cast_expression(node).add_location_to_value_if_ok(node.maybe_location());
        case Parser::PARENTHESIZED_EXPRESSION_SYMBOL:
            return bind_parenthesized_expression(node).add_location_to_value_if_ok(node.maybe_location());
        case Parser::STRING_LITERAL_SYMBOL:
            return bind_string_expression(node).add_location_to_value_if_ok(node.maybe_location());
        case Parser::NUMBER_LITERAL_SYMBOL:
        case Parser::CHAR_LITERAL_SYMBOL:
            return bind_literal_expression(node).add_location_to_value_if_ok(node.maybe_location());
        case Parser::IDENTIFIER_SYMBOL:
            return bind_identifier_expression(node).add_location_to_value_if_ok(node.maybe_location());
        case Parser::ASSIGNMENT_EXPRESSION_SYMBOL:
            if (node.child(0).symbol() == Parser::POINTER_EXPRESSION_SYMBOL)
                return bind_pointer_assignment_expression(node).add_location_to_value_if_ok(node.maybe_location());
            else if(node.child(0).symbol() == Parser::SUBSCRIPT_EXPRESSION_SYMBOL)
                return bind_subscript_assignment_expression(node).add_location_to_value_if_ok(node.maybe_location());
            else
                return bind_assignment_expression(node).add_location_to_value_if_ok(node.maybe_location());
        case Parser::CALL_EXPRESSION_SYMBOL:
            return bind_call_expression(node).add_location_to_value_if_ok(node.maybe_location());
        case Parser::POINTER_EXPRESSION_SYMBOL:
            if (node.value().starts_with('&'))
                return bind_reference_expression(node).add_location_to_value_if_ok(node.maybe_location());
            else
                return bind_dereference_expression(node).add_location_to_value_if_ok(node.maybe_location());
        case Parser::SIZEOF_EXPRESSION_SYMBOL:
            return bind_sizeof_expression(node).add_location_to_value_if_ok(node.maybe_location());
        case Parser::SUBSCRIPT_EXPRESSION_SYMBOL:
            return bind_subscript_expression(node).add_location_to_value_if_ok(node.maybe_location());
        case Parser::UPDATE_EXPRESSION_SYMBOL:
            return bind_update_expression(node).add_location_to_value_if_ok(node.maybe_location());
        case Parser::UNARY_EXPRESSION_SYMBOL:
            return bind_unary_expression(node).add_location_to_value_if_ok(node.maybe_location());
        default:
            SCC_NOT_IMPLEMENTED_WARN(node.symbol_name());
            break;
        }

        SCC_BINDER_RESULT_TYPE(bind_expression);
        return binding::BinderResult<ResultType>::error(binding::BinderError(ErrorKind::ReachedUnreachableError, node));

    }
}