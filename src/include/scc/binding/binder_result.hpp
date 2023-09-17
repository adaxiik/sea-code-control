#pragma once
#include <memory>
#include <vector>

#include "location.hpp"
#include "binding/bound_node.hpp"
namespace scc
{
    namespace binding
    {
        struct BinderError
        {
            enum class BinderErrorKind
            {
                None = 0,
                InvalidNumberLiteralError,
                ReachedUnreachableError,
                InvalidOperationError,
                UnknownSymbolError,
                EmptyExpressionError,
                InvalidInitializerError,
                FailedToCreateVariableError,
                FunctionArgumentCountMismatchError,
                FunctionSignatureMismatchError,
                ReturnStatementOutsideFunctionError,
                FunctionAlreadyDefinedError,
                ReturnStatementMissingExpressionError,
                // TODOO: Variable already exists / Variable does not exist
                COUNT
            };

            BinderErrorKind kind;
            Location location;

            BinderError(BinderErrorKind kind, Location location) : kind(kind), location(location) {}
            BinderError(BinderErrorKind kind, const TreeNode& node) : kind(kind), location(node.location()) {}

            BinderError(const BinderError& other) = default;
            BinderError(BinderError&& other) = default;
            BinderError& operator=(const BinderError& other) = default;
            BinderError& operator=(BinderError&& other) = default;


            static BinderError None() { return BinderError(BinderErrorKind::None, {0,0}); }

            bool operator==(const BinderError& other) const { return kind == other.kind; }
        };
        
        template<typename T>
        class BinderResult
        {
            std::vector<std::string> m_diagnostics;
            std::unique_ptr<T> m_value;
            BinderError m_binder_error;

            BinderResult(std::vector<std::string> diagnostics, std::unique_ptr<T> value, BinderError binder_error)
                : m_diagnostics(diagnostics)
                , m_value(std::move(value))
                , m_binder_error(binder_error) {}
            
        public:
            using type = T;
            static BinderResult ok(std::unique_ptr<T> value) { return BinderResult({}, std::move(value), BinderError::None()); }
            static BinderResult error(BinderError binder_error) { return BinderResult({}, nullptr, binder_error); }
            static BinderResult error(BinderError binder_error, std::vector<std::string> diagnostics) { return BinderResult(diagnostics, nullptr, binder_error); }

            // implicit upcast
            template<typename U>
            BinderResult(BinderResult<U>&& other)
                : m_diagnostics(std::move(other.get_diagnostics()))
                , m_binder_error(other.get_error()) 
                {

                    if constexpr (std::is_base_of_v<T, U>)
                    {
                        m_value = std::unique_ptr<T>(static_cast<T*>(other.release_value().release()));
                    }
                    else 
                    {
                        if(other.has_value())
                            throw std::runtime_error("BinderResult: Cannot implicitly upcast to a different type Base: " + std::string(typeid(T).name()) + " Derived: " + std::string(typeid(U).name()));
                        
                        m_value = nullptr;
                    }
                }
                
            BinderResult(BinderResult&& other) = default;

            BinderResult(std::unique_ptr<T> value)
                : m_diagnostics({})
                , m_value(std::move(value))
                , m_binder_error(BinderError::None()) {}
           
            
            BinderResult(BinderError binder_error) 
                : m_diagnostics({})
                , m_value(nullptr)
                , m_binder_error(binder_error) {}
            
            BinderResult(BinderError binder_error, std::vector<std::string> diagnostics) 
                : m_diagnostics(diagnostics)
                , m_value(nullptr)
                , m_binder_error(binder_error) {}
            
            bool is_ok() const { return m_binder_error == BinderError::None(); }
            bool is_error() const { return !is_ok(); }

            T* get_value() const { return m_value.get(); }
            std::unique_ptr<T> release_value() 
            { 
                std::unique_ptr<T> value = std::move(m_value);
                return value;
            }

            bool has_value() const { return m_value != nullptr; }
            BinderError get_error() const { return m_binder_error; }

            void add_diagnostic(const std::string& diagnostic) { m_diagnostics.push_back(diagnostic); }
            const std::vector<std::string>& get_diagnostics() const { return m_diagnostics; }
            
        };

    }
}