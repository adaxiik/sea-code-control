#pragma once
#include "variable.hpp"
#include <map>
#include <vector>
#include <optional>
#include "interpreter_result.hpp"

namespace scc
{
    template <typename T>
    class Scope
    {
    protected:
        std::map<std::string, T> m_scope;

    public:
        virtual ~Scope() = default;
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
}
