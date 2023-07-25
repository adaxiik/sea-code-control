#pragma once

namespace scc
{
    namespace binding
    {
        enum class BoundNodeKind
        {
            ExpressionStatement,
            BlockStatement,

            BinaryExpression,
            LiteralExpression,
            
            COUNT,
        };

        struct BoundNode
        {
            // BoundNodeKind bound_node_kind;
            virtual ~BoundNode() = default;
            virtual BoundNodeKind bound_node_kind() const = 0;
        };
    }
}