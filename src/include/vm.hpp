#pragma once

#include <vector>
#include <memory>
#include <cstdint>
#include <map>
#include <optional>
#include <cassert>
#include "instruction.hpp"
#include "type.hpp"

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

        struct Padding
        {
            using PaddingType = uint32_t;
            constexpr static size_t Size = sizeof(PaddingType);
            PaddingType data;
        };
        static_assert(sizeof(Padding) == Padding::Size, "Padding is not the correct size");

        enum class Error
        {
            None,
            VariableAlreadyExists,
        };

        using relptr_t = size_t;

        struct VariablePrimitive
        {
            type::Type type;
            relptr_t pointer;
        };
        using Variable = std::variant<VariablePrimitive>;

        struct Scope
        {
            std::map<std::string, relptr_t> m_variables;
        };

        struct ScopeStack
        {
            std::vector<Scope> m_scopes;

            std::optional<relptr_t> operator[](const std::string &name) const
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
            Error create_variable(const std::string &name, relptr_t pointer)
            {
                assert(!m_scopes.empty() && "No scope to create variable in");
                std::optional<relptr_t> var{this->operator[](name)};
                
                if (var.has_value())
                    return Error::VariableAlreadyExists;
                
                m_scopes.back().m_variables[name] = pointer;

                return Error::None;
            }
        };

        constexpr size_t VM_STACK_SIZE = (1024 * 8) / Padding::Size; // 8 KiB
        struct VM
        {
            std::unique_ptr<Padding[]> m_stack;
            std::vector<Instruction> m_instructions;
            size_t m_instruction_pointer;
            size_t m_stack_pointer;

            explicit VM(std::vector<Instruction> instructions);

            Error execute_current();
            Error run();
        };

    }
}