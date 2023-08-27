#pragma once
#include "variable.hpp"
#include <map>
#include <vector>
#include <optional>
#include "interpreter_result.hpp"

// TODOO: Move implementation to .cpp file
namespace scc
{
    template <typename T>
    class Scope
    {
    protected:
        std::map<std::string, T> m_scope;
        virtual ~Scope() = default;

    public:
        using value_type = T;
        std::map<std::string, T> &ref_scopes()
        {
            return m_scope;
        }

        T* get_from_scope(const std::string &name)
        {
            auto var = m_scope.find(name);
            if (var != m_scope.end())
                return &var->second;
            return nullptr;
        }


    };

    template <typename T>
    class ScopeStack
    {
    protected:
        std::vector<T> m_scopes;
        virtual ~ScopeStack() = default;

    public:
        auto get_from_scopestack(const std::string &name) -> typename T::value_type*
        {
            for (auto it = m_scopes.rbegin(); it != m_scopes.rend(); it++)
            {
                auto var = it->get_from_scope(name);
                if (var != nullptr)
                    return var;
            }

            return nullptr;
        }

        virtual void push() = 0;
        void pop() { m_scopes.pop_back(); }
    
    };

    class InterpreterScope : public Scope<Variable>
    {
        Memory::address_t m_start_address;
        Memory::address_t m_current_address;

    public:
        InterpreterScope(Memory::address_t start_address);
        virtual ~InterpreterScope() = default;

        Memory::address_t start_address() const;
        Memory::address_t current_address() const;

        // TODOO: padding?
        void shift_current_address(Memory::address_t shift);
        // we shifting current address to the left
        // | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 10 |
        //                                            ^ start_address

        InterpreterScope next_scope() const;
    };

    class InterpreterScopeStack : public ScopeStack<InterpreterScope>
    {

    public:
        InterpreterScopeStack(Memory::address_t start_address);
        virtual ~InterpreterScopeStack() = default;
        virtual void push() override;
        

        InterpreterResult create_variable(const std::string &name, Type type, size_t size_bytes, bool is_constant);
        std::vector<InterpreterScope> &ref_scopes();
    };
}
