#pragma once
#include "type.hpp"

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