#pragma once
#include "variable.hpp"
#include <map>
#include <vector>
#include <optional>
#include "interpreter_result.hpp"

namespace scc
{
    class Scope
    {
        std::map<std::string, Variable> m_variables;

    public:
        std::optional<Variable> get_variable(const std::string &name)
        {
            auto var = m_variables.find(name);
            if (var != m_variables.end())
                return var->second;

            return std::nullopt;
        }

        std::map<std::string, Variable> &ref_variables() { return m_variables; }
        const std::map<std::string, Variable> &ref_variables() const { return m_variables; }
    };

    class ScopeStack
    {
        std::vector<Scope> m_scopes;

    public:
        std::optional<Variable> get_variable(const std::string &name)
        {
            for (auto it = m_scopes.rbegin(); it != m_scopes.rend(); it++)
            {
                auto var = it->get_variable(name);
                if (var.has_value())
                    return var;
            }

            return std::nullopt;
        }

        void push() { m_scopes.push_back({}); }
        void pop() { m_scopes.pop_back(); }
        InterpreterResult create_variable(const std::string &name, Variable variable)
        {
            if (m_scopes.empty()) // This should be unreachable, but you never know
                return InterpreterError::RuntimeError;

            std::optional<Variable> var{get_variable(name)};

            if (var.has_value())
                return InterpreterError::VariableAlreadyExistsError;

            m_scopes.back().ref_variables()[name] = variable;
            return InterpreterError::None;
        }

        std::vector<Scope> &ref_scopes() { return m_scopes; }
        const std::vector<Scope> &ref_scopes() const { return m_scopes; }
    };
}
