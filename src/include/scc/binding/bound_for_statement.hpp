#pragma once
#include <memory>
#include <utility>
#include <optional>
#include "binding/bound_statement.hpp"
#include "binding/bound_expression.hpp"
#include "binding/bound_block_statement.hpp"

namespace scc
{
    namespace binding
    {
        struct BoundForStatement: public BoundStatement
        {
            std::unique_ptr<BoundNode> initializer; // can be statement or expression (int a; vs a;)
            std::unique_ptr<BoundExpression> condition;
            std::unique_ptr<BoundExpression> increment;
            std::unique_ptr<BoundBlockStatement> body;

            BoundForStatement(
                std::unique_ptr<BoundNode> initializer,
                std::unique_ptr<BoundExpression> condition,
                std::unique_ptr<BoundExpression> increment,
                std::unique_ptr<BoundBlockStatement> body
            )
            : initializer(std::move(initializer))
            , condition(std::move(condition))
            , increment(std::move(increment))
            , body(std::move(body))
            {
            }

            virtual BoundNodeKind bound_node_kind() const override
            {
                return BoundNodeKind::ForStatement;
            }
        };
    }
}