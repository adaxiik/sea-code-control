#pragma once

#include "binding/bound_node.hpp"
#include "binding/bound_statement.hpp"

#include <memory>

namespace scc
{
    namespace binding
    {
        struct BoundBreakStatement : public BoundStatement
        {
            virtual BoundNodeKind bound_node_kind() const override
            {
                return BoundNodeKind::BreakStatement;
            }
        };
    } 
}