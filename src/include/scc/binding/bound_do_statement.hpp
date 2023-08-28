#pragma once

#include "binding/bound_node.hpp"
#include "binding/bound_statement.hpp"

#include <memory>

namespace scc
{
    namespace binding
    {
        struct BoundDoStatement : public BoundStatement
        {
            std::unique_ptr<BoundExpression> condition;
            std::unique_ptr<BoundStatement> body;

            BoundDoStatement(std::unique_ptr<BoundExpression> condition, std::unique_ptr<BoundStatement> body)
                :  condition(std::move(condition)), body(std::move(body)) {}

            virtual BoundNodeKind bound_node_kind() const override
            {
                return BoundNodeKind::DoStatement;
            }
 
        };
    } 
}