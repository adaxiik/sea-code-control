#pragma once
#include "binding/bound_expression.hpp"
namespace scc
{
    namespace binding
    {
        struct BoundPointerExpression : public BoundExpression
        {
            std::string identifier;
            BoundPointerExpression(const std::string &identifier, const Type &result_type)
                : BoundExpression(result_type), identifier(identifier)
            {
            }

            virtual BoundNodeKind bound_node_kind() const override
            {
                // TODO: Change this
                return BoundNodeKind::PointerExpression; 
            }
        };
    } 
}