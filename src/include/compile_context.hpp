#pragma once
#include "type.hpp"
#include <map>
#include <vector>
#include <string>
#include <stack>

namespace scc
{
    struct CompileScope
    {
        std::map<std::string, type::Type> variables;
    };

    struct CompileScopeStack
    {
        std::vector<CompileScope> scopes;

        void push() { scopes.push_back({}); }
        void pop() { scopes.pop_back(); }
        void create_variable(const std::string &name, type::Type type)
        {
            assert(!scopes.empty() && "No scope to create variable in");
            scopes.back().variables[name] = type;
        }
        std::optional<type::Type> get_variable_type(const std::string &name)
        {
            for (auto it = scopes.rbegin(); it != scopes.rend(); it++)
            {
                auto var = it->variables.find(name);
                if (var != it->variables.end())
                    return var->second;
            }

            return std::nullopt;
        }
    };


    struct CompileContext
    {
        type::Type current_type;
        CompileScopeStack scope_stack;

        void reset()
        {
            scope_stack = std::move(CompileScopeStack{});
        }
    };
}
