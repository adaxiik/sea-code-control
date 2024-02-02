#pragma once
#include "binding/bound_expression.hpp"
namespace scc
{
    namespace binding
    {
        struct BoundPrintfExpression : public BoundExpression
        {
            std::unique_ptr<BoundExpression> format;
            std::vector<std::unique_ptr<BoundExpression>> arguments;

            BoundPrintfExpression(std::unique_ptr<BoundExpression> format, std::vector<std::unique_ptr<BoundExpression>> arguments)
                : BoundExpression(Type(Type::PrimitiveType::I32)), format(std::move(format)), arguments(std::move(arguments))
            {
            }

            virtual BoundNodeKind bound_node_kind() const override
            {
                return BoundNodeKind::PrintfExpression;
            }

            size_t arg_count() const
            {
                return arguments.size();
            }
        };
    }
}