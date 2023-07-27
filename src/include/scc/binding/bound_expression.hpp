#pragma once
#include "type.hpp"
#include "binding/bound_node.hpp"

namespace scc
{
    namespace binding
    {
        struct BoundExpression : public BoundNode
        {
            Type type;
            BoundExpression(Type type)
                : type(type)
            {
            }
        };
    } 
}