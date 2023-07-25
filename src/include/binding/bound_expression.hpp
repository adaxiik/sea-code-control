#pragma once
#include "type.hpp"

namespace scc
{
    namespace binding
    {
        struct BoundExpression : public BoundNode
        {
            type::Type type;
            BoundExpression(const type::Type& type)
                : type(type)
            {
            }
        };
    } 
}