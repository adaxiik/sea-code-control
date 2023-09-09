#pragma once
#include <memory>
#include <optional>
#include "binding/bound_expression.hpp"
#include "binding/bound_node.hpp"

namespace scc
{
    namespace binding
    {
        struct BoundCallExpression: public BoundExpression
        {
            std::string function_name;
            std::vector<std::unique_ptr<BoundExpression>> arguments;

            BoundCallExpression(Type type, std::string function_name, std::vector<std::unique_ptr<BoundExpression>> arguments)
                : BoundExpression(type)
                , function_name(std::move(function_name))
                , arguments(std::move(arguments))
            {
            }

            virtual BoundNodeKind bound_node_kind() const override
            {
                return BoundNodeKind::CallExpression;
            }
        };
    }
}