#pragma once
#include "binding/bound_expression.hpp"
namespace scc
{
    namespace binding
    {
        struct BoundPointerAssignmentExpression : public BoundExpression
        {
            std::unique_ptr<BoundExpression> address_expression;
            std::unique_ptr<BoundExpression> expression;

            BoundPointerAssignmentExpression(std::unique_ptr<BoundExpression> address_expression, std::unique_ptr<BoundExpression> expression, Type target_type)
                : BoundExpression(std::move(target_type))
                , address_expression(std::move(address_expression))
                , expression(std::move(expression))
            {
            }


            virtual BoundNodeKind bound_node_kind() const override
            {
                return BoundNodeKind::PointerAssignmentExpression; 
            }
        };
    } 
}