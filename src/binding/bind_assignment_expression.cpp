#include "binding/binder.hpp"

namespace scc
{
    binding::BinderResult<binding::BoundAssignmentExpression> Binder::bind_assignment_expression(const TreeNode &node)
    {
        SCC_BINDER_RESULT_TYPE(bind_assignment_expression);  
        SCC_ASSERT_NODE_SYMBOL(Parser::ASSIGNMENT_EXPRESSION_SYMBOL);
        SCC_ASSERT_CHILD_COUNT(node, 3);

        // assignment_expression ==>       a+=9
        // ├── identifier ==>      a
        // ├── += ==>      +=
        // └── number_literal ==>  9
        // we will convert everything into a pure assignment expression
        // a += 9 ==> a = a + 9
        std::string op_kind = node.child(1).value();

        // TODOOO: int* a;
        // a = 5; // error .. to pointers I dont want to allow literal assignment
        // or we can check it in interpreter?

        // assignment_expression ==>       data[0] = ...
        // ├── subscript_expression ==>    data[0]
        // │   ├── identifier ==>  data
        // │   └── number_literal ==>      0
        // └── char_literal ==>    'b'

        std::string identifier = node.first_child().value();
        if (m_macros.find(identifier) != m_macros.end())
        {
            const std::string& macro_value = m_macros[identifier];
            Parser parser = Parser();
            ParserResult parser_result = parser.parse(macro_value, true);
            if (parser_result.has_error())
            {
                SCC_BINDER_RESULT_TYPE(bind_assignment_expression);  
                auto error = binding::BinderResult<ResultType>(binding::BinderError(ErrorKind::DefineMacroParseError, node));
                error.add_diagnostic("Failed to parse macro value: " + macro_value);
                return error;
            }
            identifier = parser_result.root_node().first_named_child().value();
        }

        Type* type_ptr = m_scope_stack.get_from_scopestack(identifier);
        if (not type_ptr)
        {
            auto error = binding::BinderResult<ResultType>(binding::BinderError(ErrorKind::UnknownSymbolError, node));
            error.add_diagnostic("Unknown identifier: " + identifier);
            return error;
        }
        Type type = *type_ptr;

        if (op_kind == "=")
        {
            auto expression = bind_expression(node.last_child());
            BUBBLE_ERROR(expression);
            auto casted_expression = std::make_unique<binding::BoundCastExpression>(expression.release_value(), type);
            return std::make_unique<binding::BoundAssignmentExpression>(std::move(identifier), type, std::move(casted_expression));
        }

        auto operator_kind_opt = Binder::operation_kind_from_string(op_kind);
        if (not operator_kind_opt.has_value())
            return binding::BinderResult<ResultType>::error(binding::BinderError(ErrorKind::ReachedUnreachableError, node));

        auto operator_kind = operator_kind_opt.value();
        auto expression = bind_expression(node.last_child());
        BUBBLE_ERROR(expression);
        auto binary_expression = std::make_unique<binding::BoundBinaryExpression>(std::make_unique<binding::BoundIdentifierExpression>(identifier, type)
                                                                                , expression.release_value()
                                                                                , type
                                                                                , operator_kind);
        auto casted_expression = std::make_unique<binding::BoundCastExpression>(std::move(binary_expression), type);

        return std::make_unique<binding::BoundAssignmentExpression>(std::move(identifier)
                                                                  , type
                                                                  , std::move(casted_expression));

    }
}