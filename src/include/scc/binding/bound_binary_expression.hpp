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
        struct BoundBinaryExpression : public BoundExpression
        {
            enum class OperatorKind
            {
                // arithmetic
                Addition,
                Subtraction,
                Multiplication,
                Division,
                Modulo,
                BitwiseAnd,
                BitwiseOr,
                BitwiseXor,
                BitwiseShiftLeft,
                BitwiseShiftRight,

                // logical
                LogicalAnd,
                LogicalOr,

                // comparison
                Equals,
                NotEquals,
                LessThan,
                GreaterThan,
                LessThanOrEqual,
                GreaterThanOrEqual,

                COUNT
            };

            std::unique_ptr<BoundExpression> left;
            std::unique_ptr<BoundExpression> right;
            OperatorKind op_kind;

            BoundBinaryExpression(std::unique_ptr<BoundExpression> left, std::unique_ptr<BoundExpression> right, Type result_type, OperatorKind op_kind)
                : BoundExpression(result_type) 
                , left(std::move(left))
                , right(std::move(right))
                , op_kind(op_kind)
            {
            }

            static std::optional<Type> deduce_type(Type left, Type right, OperatorKind op_kind);

            virtual BoundNodeKind bound_node_kind() const override
            {
                return BoundNodeKind::BinaryExpression;
            }
        };
    }
}