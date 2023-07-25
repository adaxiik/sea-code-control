#pragma once

#include "binding/bound_node.hpp"
#include <vector>
#include <memory>

namespace scc
{
    namespace binding
    {
        struct BoundBlockStatement final : public BoundNode
        {
            std::vector<std::unique_ptr<BoundNode>> statements;

            virtual BoundNodeKind bound_node_kind() const override
            {
                return BoundNodeKind::BlockStatement;
            }
        };
    } 
}