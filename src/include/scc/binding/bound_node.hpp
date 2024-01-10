#pragma once
#include "location.hpp"
#include <optional>
namespace scc
{
    namespace binding
    {
        enum class BoundNodeKind
        {
            ExpressionStatement = 0,
            BlockStatement,
            VariableDeclarationStatement,
            IfStatement,
            WhileStatement,
            DoStatement,
            BreakStatement,
            ContinueStatement,
            FunctionStatement,
            ReturnStatement,
            ForStatement,


            // Expressions .. see is_statement() 
            BinaryExpression,   // binary_expression should be the first expression, see EXPRESSION_COUNT
            LiteralExpression,
            CastExpression,
            ParenthesizedExpression,
            IdentifierExpression,
            AssignmentExpression,
            CallExpression,
            PointerExpression,



            COUNT,
        };

        constexpr auto EXPRESSION_COUNT = static_cast<int>(BoundNodeKind::COUNT) - static_cast<int>(BoundNodeKind::BinaryExpression);
        constexpr auto STATEMENT_COUNT = static_cast<int>(BoundNodeKind::COUNT) - EXPRESSION_COUNT;

        struct BoundNode
        {
            std::optional<Location> location = std::nullopt;

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

            void add_location(Location loc)
            {
                location = loc;
            }
        };
    }
}