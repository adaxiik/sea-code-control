#include "binding/binder.hpp"

namespace scc
{
    
    binding::BinderResult<binding::BoundExpression> Binder::bind_update_expression(const TreeNode &node)
    {
        SCC_BINDER_RESULT_TYPE(bind_update_expression);
        SCC_ASSERT_NODE_SYMBOL(Parser::UPDATE_EXPRESSION_SYMBOL);
        SCC_ASSERT_CHILD_COUNT(node, 2);
        // unnamed child:
        // expression_statement ==>    ++i;
        // ├── update_expression ==>   ++i
        // │   ├── ++ ==>      ++
        // │   └── identifier ==>      i
        // └── ; ==>   ;

        bool pre = node.first_child().value() == "++" or node.first_child().value() == "--";
        bool is_increment = node.first_child().value() == "++" or node.last_child().value() == "++";


        // ++a => a = a + 1
        // a++ => (a = a + 1) - 1
        // --a => a = a - 1
        // a-- => (a = a - 1) + 1

        auto bound_expression_result = bind_expression(pre ? node.last_child() : node.first_child());
        BUBBLE_ERROR(bound_expression_result);

        std::unique_ptr<binding::BoundExpression> bound_expression = bound_expression_result.release_value();
        binding::BoundNodeKind expression_kind = bound_expression->bound_node_kind();

        // only used for identifier expression
        std::string identifier;
        if (expression_kind == binding::BoundNodeKind::IdentifierExpression)
            identifier = static_cast<binding::BoundIdentifierExpression*>(bound_expression.get())->identifier;

        std::unique_ptr<binding::BoundExpression> casted_one;

        Type bound_expression_type = bound_expression->type;

        if (bound_expression->type.is_pointer())
            casted_one = std::make_unique<binding::BoundLiteralExpression>(1);
        else        
            casted_one = std::make_unique<binding::BoundCastExpression>(std::make_unique<binding::BoundLiteralExpression>(1), bound_expression->type);
        
        auto pre_binary_expression = std::make_unique<binding::BoundBinaryExpression>(
            std::move(bound_expression),
            std::move(casted_one),
            bound_expression_type,
            is_increment ? binding::BoundBinaryExpression::OperatorKind::Addition : binding::BoundBinaryExpression::OperatorKind::Subtraction
        );

        // this became lil bit (more) dirty, but I dont care enough at this point :/
        // basically, there is difference between ++a and ++*a (or a++ and *a++)

        if (pre and expression_kind == binding::BoundNodeKind::IdentifierExpression)
        {
            // ++a => a = a + 1

            Type* type_ptr = m_scope_stack.get_from_scopestack(identifier);
            if (not type_ptr)
            {
                auto error = binding::BinderResult<ResultType>(binding::BinderError(ErrorKind::UnknownSymbolError, node));
                error.add_diagnostic("Unknown identifier: " + identifier);
                return error;
            }
            Type type = *type_ptr;
            auto casted_pre_binary_expression = std::make_unique<binding::BoundCastExpression>(std::move(pre_binary_expression), type);
            auto assignment = std::make_unique<binding::BoundAssignmentExpression>(std::move(identifier), type, std::move(casted_pre_binary_expression));
            
            return std::unique_ptr<binding::BoundExpression>(std::move(assignment));
        }
        else if (pre and expression_kind == binding::BoundNodeKind::DereferenceExpression)
        {
            if (node.last_child().symbol() == Parser::POINTER_EXPRESSION_SYMBOL)
            {
                auto bound_address_expression = bind_expression(node.last_child().first_named_child());
                if (not bound_address_expression.get_value()->type.is_pointer())
                {
                    auto error = binding::BinderResult<ResultType>(binding::BinderError(ErrorKind::InvalidOperationError, node));
                    error.add_diagnostic("Cannot dereference non-pointer type");
                    return error;
                }

                Type target_type = bound_address_expression.get_value()->type;
                target_type.modifiers.pop_back();

                auto casted_pre_binary_expression = std::make_unique<binding::BoundCastExpression>(std::move(pre_binary_expression), target_type);
                auto assignment = std::make_unique<binding::BoundPointerAssignmentExpression>(bound_address_expression.release_value(), std::move(casted_pre_binary_expression), target_type);
                return std::unique_ptr<binding::BoundExpression>(std::move(assignment));
            }
            else if (node.last_child().symbol() == Parser::SUBSCRIPT_EXPRESSION_SYMBOL)
            {
                // ++a[0]
                auto bound_subscript_expression = bind_subscript_expression(node.last_child());
                BUBBLE_ERROR(bound_subscript_expression);

                // we want to skip dereference, so we just take the underlying expression
                auto left_expression = std::move(bound_subscript_expression.get_value()->expression);
                
                Type target_type = left_expression->type;
                target_type.modifiers.pop_back();

                auto casted_pre_binary_expression = std::make_unique<binding::BoundCastExpression>(std::move(pre_binary_expression), target_type);
                auto assignment = std::make_unique<binding::BoundPointerAssignmentExpression>(std::move(left_expression), std::move(casted_pre_binary_expression), target_type);
                return std::unique_ptr<binding::BoundExpression>(std::move(assignment));
            }
            else
            {
                auto error = binding::BinderResult<ResultType>(binding::BinderError(ErrorKind::InvalidUpdateExpression, node));
                error.add_diagnostic("Invalid update expression \"" + node.value() + "\"");
                return error;
            }
        }
        else if (expression_kind == binding::BoundNodeKind::IdentifierExpression)
        {
            // a++ => (a = a + 1) - 1
            
            Type* type_ptr = m_scope_stack.get_from_scopestack(identifier);
            if (not type_ptr)
            {
                auto error = binding::BinderResult<ResultType>(binding::BinderError(ErrorKind::UnknownSymbolError, node));
                error.add_diagnostic("Unknown identifier: " + identifier);
                return error;
            }
            Type type = *type_ptr;
            auto casted_pre_binary_expression = std::make_unique<binding::BoundCastExpression>(std::move(pre_binary_expression), type);
            auto assignment = std::make_unique<binding::BoundAssignmentExpression>(std::move(identifier), type, std::move(casted_pre_binary_expression));
            std::unique_ptr<binding::BoundExpression> casted_one_again;
            if (bound_expression_type.is_pointer())
                casted_one_again = std::make_unique<binding::BoundLiteralExpression>(1);
            else        
                casted_one_again = std::make_unique<binding::BoundCastExpression>(std::make_unique<binding::BoundLiteralExpression>(1), bound_expression_type);
            
            auto post_binary_expression = std::make_unique<binding::BoundBinaryExpression>(
                std::move(assignment),
                std::move(casted_one_again),
                bound_expression_type,
                is_increment ? binding::BoundBinaryExpression::OperatorKind::Subtraction : binding::BoundBinaryExpression::OperatorKind::Addition
            );
            return std::unique_ptr<binding::BoundExpression>(std::move(post_binary_expression));
        }
        else if (expression_kind == binding::BoundNodeKind::DereferenceExpression)
        {
            if (node.first_child().symbol() == Parser::POINTER_EXPRESSION_SYMBOL)
            {
                // *a++ actually means *(a++) so this should be unreachable

                auto error = binding::BinderResult<ResultType>(binding::BinderError(ErrorKind::InvalidUpdateExpression, node));
                error.add_diagnostic("This should be unreachable :) \"" + node.value() + "\"");
                return error;
            }
            else if (node.first_child().symbol() == Parser::SUBSCRIPT_EXPRESSION_SYMBOL)
            {
                // a[0]++ => (a[0] = a[0] + 1) - 1
                auto bound_subscript_expression = bind_subscript_expression(node.first_child());
                BUBBLE_ERROR(bound_subscript_expression);

                // we want to skip dereference, so we just take the underlying expression
                auto left_expression = std::move(bound_subscript_expression.get_value()->expression);

                Type target_type = left_expression->type;
                target_type.modifiers.pop_back();

                auto casted_pre_binary_expression = std::make_unique<binding::BoundCastExpression>(std::move(pre_binary_expression), target_type);
                auto assignment = std::make_unique<binding::BoundPointerAssignmentExpression>(std::move(left_expression), std::move(casted_pre_binary_expression), target_type);
                std::unique_ptr<binding::BoundExpression> casted_one_again;
                if (bound_expression_type.is_pointer())
                    casted_one_again = std::make_unique<binding::BoundLiteralExpression>(1);
                else        
                    casted_one_again = std::make_unique<binding::BoundCastExpression>(std::make_unique<binding::BoundLiteralExpression>(1), bound_expression_type);

                auto post_binary_expression = std::make_unique<binding::BoundBinaryExpression>(
                    std::move(assignment),
                    std::move(casted_one_again),
                    bound_expression_type,
                    is_increment ? binding::BoundBinaryExpression::OperatorKind::Subtraction : binding::BoundBinaryExpression::OperatorKind::Addition
                );
                return std::unique_ptr<binding::BoundExpression>(std::move(post_binary_expression));
            }
            else
            {
                auto error = binding::BinderResult<ResultType>(binding::BinderError(ErrorKind::InvalidUpdateExpression, node));
                error.add_diagnostic("Invalid update expression \"" + node.value() + "\"");
                return error;
            }
        }
        else 
        {
            auto error = binding::BinderResult<ResultType>(binding::BinderError(ErrorKind::InvalidUpdateExpression, node));
            error.add_diagnostic("Invalid update expression \"" + node.value() + "\"");
            return error;
        }
    }
}