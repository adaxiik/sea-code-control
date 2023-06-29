#pragma once

#include <vector>
#include <memory>
#include <cstdint>
#include <map>
#include <optional>
#include <cassert>
#include "instruction.hpp"
#include "type.hpp"
#include "padding.hpp"

namespace scc
{
    namespace vm
    {

        // https://en.cppreference.com/w/cpp/utility/variant/visit
        template <class... Ts>
        struct overloaded : Ts...
        {
            using Ts::operator()...;
        };
        template <class... Ts>
        overloaded(Ts...) -> overloaded<Ts...>;

        

        enum class Error
        {
            None,
            VariableAlreadyExists,
            VariableDoesNotExist,
            StackOverflow,
            StackUnderflow,
            IllegalMemoryAccess,
        };

        using relptr_t = int64_t;

        struct Variable
        {
            type::Type type;
            relptr_t pointer;
        };

        struct Scope
        {
            std::map<std::string, Variable> m_variables;
        };

        struct ScopeStack
        {
            std::vector<Scope> m_scopes;

            std::optional<Variable> operator[](const std::string &name) 
            {
                for (auto it = m_scopes.rbegin(); it != m_scopes.rend(); it++)
                {
                    auto var = it->m_variables.find(name);
                    if (var != it->m_variables.end())
                        return var->second;
                }

                return std::nullopt;
            }

            void push(){ m_scopes.push_back({}); }
            void pop(){ m_scopes.pop_back(); }
            Error create_variable(const std::string &name, Variable variable)
            {
                assert(!m_scopes.empty() && "No scope to create variable in");
                std::optional<Variable> var{this->operator[](name)};
                
                if (var.has_value())
                    return Error::VariableAlreadyExists;
                
                m_scopes.back().m_variables[name] = std::move(variable);

                return Error::None;
            }
        };

        constexpr size_t VM_STACK_SIZE = 1024 * 8; // 8 KiB
        struct VM
        {
            std::unique_ptr<char[]> m_stack;
            std::vector<Instruction> m_instructions;
            ScopeStack m_scope_stack;
            size_t m_instruction_pointer;
            int64_t m_stack_pointer;

            explicit VM(std::vector<Instruction> instructions);

            Error execute_current();
            Error run();
        public:
            Error pad_stack(size_t want_to_write_bytes);
            Error stack_push(uint64_t value);
            Error stack_pop(uint64_t &value);
        };

    }
}