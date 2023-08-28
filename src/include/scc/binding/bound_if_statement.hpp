#pragma once

#include "binding/bound_node.hpp"
#include "binding/bound_statement.hpp"

#include <memory>

namespace scc
{
    namespace binding
    {
        struct BoundIfStatement : public BoundStatement
        {
            std::unique_ptr<BoundExpression> condition;
            std::unique_ptr<BoundStatement> then_statement;
            std::unique_ptr<BoundStatement> else_statement;

            BoundIfStatement(std::unique_ptr<BoundExpression> condition, std::unique_ptr<BoundStatement> then_statement, std::unique_ptr<BoundStatement> else_statement)
                : condition(std::move(condition)), then_statement(std::move(then_statement)), else_statement(std::move(else_statement))
            {
            }

            BoundIfStatement(std::unique_ptr<BoundExpression> condition, std::unique_ptr<BoundStatement> then_statement)
                : condition(std::move(condition)), then_statement(std::move(then_statement))
            {
            }


            virtual BoundNodeKind bound_node_kind() const override
            {
                return BoundNodeKind::IfStatement;
            }

            bool has_else() const
            {
                return else_statement != nullptr;
            }
            
        };
    } 
}