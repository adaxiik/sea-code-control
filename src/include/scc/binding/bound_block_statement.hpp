#pragma once

#include "binding/bound_statement.hpp"
#include <vector>
#include <memory>

namespace scc
{
    namespace binding
    {
        struct BoundBlockStatement final : public BoundStatement
        {
            std::vector<std::unique_ptr<BoundStatement>> statements;

            virtual BoundNodeKind bound_node_kind() const override
            {
                return BoundNodeKind::BlockStatement;
            }
        };
    } 
}