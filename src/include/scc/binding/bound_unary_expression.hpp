#pragma once
#include <memory>
#include <optional>
#include "binding/bound_expression.hpp"
#include "binding/bound_node.hpp"
#include "operation_result.hpp"
namespace scc
{
    namespace binding
    {
        struct BoundUnaryExpression : public BoundExpression
        {
            enum class OperatorKind
            {
                BinaryNot,
                LogicalNot,
                COUNT
            };

            std::unique_ptr<BoundExpression> expression;
            OperatorKind op_kind;

            BoundUnaryExpression(std::unique_ptr<BoundExpression> expression, Type result_type, OperatorKind op_kind)
                : BoundExpression(result_type)
                , expression(std::move(expression))
                , op_kind(op_kind)
            {
            }

            static std::optional<Type> deduce_type(Type expression_type, OperatorKind op_kind);

            virtual BoundNodeKind bound_node_kind() const override
            {
                return BoundNodeKind::UnaryExpression;
            }
        };
    }
}