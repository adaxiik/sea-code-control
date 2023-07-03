#pragma once
#include <map>
#include <optional>
#include <string>
#include <vector>
#include <cassert>

#include "vm/variable.hpp"
#include "vm/error.hpp"

namespace scc
{
    namespace vm
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
            
            std::map<std::string, Variable>& ref_variables() { return m_variables; }
            const std::map<std::string, Variable>& ref_variables() const { return m_variables; }

        };

        class ScopeStack
        {
            std::vector<Scope> m_scopes;
            
            public:
            std::optional<Variable> operator[](const std::string &name)
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
            Error create_variable(const std::string &name, Variable variable)
            {
                assert(!m_scopes.empty() && "No scope to create variable in");
                std::optional<Variable> var{this->operator[](name)};

                if (var.has_value())
                    return Error::VariableAlreadyExists;

                m_scopes.back().ref_variables()[name] = std::move(variable);

                return Error::None;
            }

            std::vector<Scope>& ref_scopes() { return m_scopes; }
            const std::vector<Scope>& ref_scopes() const { return m_scopes; }
        };
    }
}