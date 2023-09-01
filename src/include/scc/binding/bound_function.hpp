#pragma once
#include <string>
#include <memory>
#include <optional>
#include "binding/bound_node.hpp"
#include "binding/bound_block_statement.hpp"
#include "binding/bound_variable_declaration_statement.hpp"
#include "type.hpp"

namespace scc
{
    namespace binding
    {
        struct BoundFunction final : BoundNode
        {
            std::optional<Type> return_type;
            std::string function_name;
            std::vector<BoundVariableDeclarationStatement> parameters;
            std::unique_ptr<BoundBlockStatement> body;

            BoundFunction(std::optional<Type> return_type, std::string function_name, std::vector<BoundVariableDeclarationStatement> parameters, std::unique_ptr<BoundBlockStatement> body)
                : return_type(std::move(return_type)), function_name(std::move(function_name)), parameters(std::move(parameters)), body(std::move(body))
            {
            }

            virtual BoundNodeKind bound_node_kind() const override
            {
                return static_cast<BoundNodeKind>(0);
            }
        };
    }
}