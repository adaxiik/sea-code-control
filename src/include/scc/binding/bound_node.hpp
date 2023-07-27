#pragma once

namespace scc
{
    namespace binding
    {
        enum class BoundNodeKind
        {
            ExpressionStatement = 0,
            BlockStatement,


            // Expressions .. see is_statement() 
            BinaryExpression,   // binary_expression should be the first expression, see EXPRESSION_COUNT
            LiteralExpression,
            
            COUNT,
        };

        constexpr auto EXPRESSION_COUNT = static_cast<int>(BoundNodeKind::COUNT) - static_cast<int>(BoundNodeKind::BinaryExpression);
        constexpr auto STATEMENT_COUNT = static_cast<int>(BoundNodeKind::COUNT) - EXPRESSION_COUNT;

        struct BoundNode
        {
            // BoundNodeKind bound_node_kind;
            virtual ~BoundNode() = default;
            virtual BoundNodeKind bound_node_kind() const = 0;

            bool is_statement() const
            {
                return static_cast<int>(bound_node_kind()) < static_cast<int>(BoundNodeKind::BinaryExpression);
            }

            bool is_expression() const
            {
                return !is_statement();
            }
        };
    }
}