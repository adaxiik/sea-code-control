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
        struct BoundFunctionStatement final : BoundStatement
        {
            Type return_type;
            std::string function_name;
            std::vector<std::unique_ptr<BoundVariableDeclarationStatement>> parameters;
            std::unique_ptr<BoundBlockStatement> body;

            BoundFunctionStatement(Type return_type
                        , std::string function_name
                        , std::vector<std::unique_ptr<BoundVariableDeclarationStatement>> parameters
                        , std::unique_ptr<BoundBlockStatement> body)
                : return_type(return_type)
                , function_name(std::move(function_name))
                , parameters(std::move(parameters))
                , body(std::move(body))
            {
            }

            virtual BoundNodeKind bound_node_kind() const override
            {
                return BoundNodeKind::FunctionStatement;
            }

            bool operator==(const BoundFunctionStatement &other) const
            {
                if (return_type != other.return_type)
                    return false;
                
                if (function_name != other.function_name)
                    return false;
                
                if (parameters.size() != other.parameters.size())
                    return false;

                for (size_t i = 0; i < parameters.size(); ++i)
                {
                    if (parameters[i]->type != other.parameters[i]->type)
                        return false;
                    
                    // int fn(float a);
                    // and
                    // int fn(float b);
                    // are the same function
                    // if (parameters[i]->variable_name != other.parameters[i]->variable_name)
                    //     return false;

                    if (parameters[i]->is_constant != other.parameters[i]->is_constant)
                        return false;
                    
                }

                return true;
            }

            bool operator!=(const BoundFunctionStatement &other) const
            {
                return !(*this == other);
            }
            
        };
    }
}