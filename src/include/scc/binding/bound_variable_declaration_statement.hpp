#pragma once

#include <vector>
#include <memory>
#include <string>
#include "binding/bound_statement.hpp"
#include "type.hpp"

namespace scc
{
    namespace binding
    {
        struct BoundVariableDeclarationStatement : public BoundStatement
        {
            std::string variable_name;
            Type type;
            bool is_constant;
            bool is_global;

            BoundVariableDeclarationStatement(
                 std::string variable_name
                , Type type
                , bool is_constant = false
                , bool is_global = false)
                : variable_name(std::move(variable_name))
                , type(type)
                , is_constant(is_constant)
                , is_global(is_global)
            {
            }

            virtual BoundNodeKind bound_node_kind() const override
            {
                return BoundNodeKind::VariableDeclarationStatement;
            }
            
            enum class VariableDeclarationStatementKind
            {
                Value = 0,
                StaticArray,
                Pointer,

                COUNT
            };

            virtual VariableDeclarationStatementKind variable_declaration_statement_kind() const = 0;
            virtual size_t size_bytes() const = 0;
        };


        struct BoundVariableValueDeclarationStatement final: public BoundVariableDeclarationStatement 
        {
            std::unique_ptr<BoundExpression> initializer;

            BoundVariableValueDeclarationStatement(
                 std::string variable_name
                , Type type
                , std::unique_ptr<BoundExpression> initializer
                , bool is_constant = false
                , bool is_global = false)
                : BoundVariableDeclarationStatement(std::move(variable_name), type, is_constant, is_global)
                , initializer(std::move(initializer))
            {
            }

            BoundVariableValueDeclarationStatement(
                 std::string variable_name
                , Type type
                , bool is_constant = false
                , bool is_global = false)
                : BoundVariableDeclarationStatement(std::move(variable_name), type, is_constant, is_global)
            {
            }

            virtual VariableDeclarationStatementKind variable_declaration_statement_kind() const override
            {
                return VariableDeclarationStatementKind::Value;
            }

            virtual size_t size_bytes() const override
            {
                return type.size_bytes();
            }
        };

        struct BoundVariableStaticArrayDeclarationStatement final: public BoundVariableDeclarationStatement 
        {
            std::vector<std::unique_ptr<BoundExpression>> initializers;
            size_t array_size;

            BoundVariableStaticArrayDeclarationStatement(
                    std::string variable_name
                    , Type type
                    , std::vector<std::unique_ptr<BoundExpression>> initializer
                    , size_t array_size
                    , bool is_constant = false
                    , bool is_global = false)
                    : BoundVariableDeclarationStatement(std::move(variable_name), type, is_constant, is_global)
                    , initializers(std::move(initializer))
                    , array_size(array_size)
                {
                }
            
            BoundVariableStaticArrayDeclarationStatement(
                    std::string variable_name
                    , Type type
                    , std::vector<std::unique_ptr<BoundExpression>> initializer
                    , bool is_constant = false
                    , bool is_global = false)
                    : BoundVariableDeclarationStatement(std::move(variable_name), type, is_constant, is_global)
                    , initializers(std::move(initializer))
                    , array_size(initializer.size())
                {
                }
            BoundVariableStaticArrayDeclarationStatement(
                    std::string variable_name
                    , Type type
                    , size_t array_size
                    , bool is_constant = false
                    , bool is_global = false)
                    : BoundVariableDeclarationStatement(std::move(variable_name), type, is_constant, is_global)
                    , array_size(array_size)
                {
                }

            virtual VariableDeclarationStatementKind variable_declaration_statement_kind() const override
            {
                return VariableDeclarationStatementKind::StaticArray;
            }

            virtual size_t size_bytes() const override
            {
                return type.size_bytes() * array_size;
            }
        };

        struct BoundVariablePointerDeclarationStatement final: public BoundVariableDeclarationStatement 
        {
            std::unique_ptr<BoundExpression> initializer;

            BoundVariablePointerDeclarationStatement(
                 std::string variable_name
                , Type type
                , std::unique_ptr<BoundExpression> initializer
                , bool is_constant = false
                , bool is_global = false)
                : BoundVariableDeclarationStatement(std::move(variable_name), type, is_constant, is_global)
                , initializer(std::move(initializer))
            {
            }

            BoundVariablePointerDeclarationStatement(
                 std::string variable_name
                , Type type
                , bool is_constant = false
                , bool is_global = false)
                : BoundVariableDeclarationStatement(std::move(variable_name), type, is_constant, is_global)
            {
            }

            virtual VariableDeclarationStatementKind variable_declaration_statement_kind() const override
            {
                return VariableDeclarationStatementKind::Pointer;
            }

            virtual size_t size_bytes() const override
            {
                return type.size_bytes();
            }
        };

    }
}