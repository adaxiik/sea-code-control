#pragma once
#include "variable.hpp"
#include <map>
#include <vector>
#include <optional>
#include "interpreter_result.hpp"


// TODOO: Move implementation to .cpp file
namespace scc
{
    class InterpreterScope
    {
        std::map<std::string, Variable> m_variables;
        Memory::address_t m_start_address;
        Memory::address_t m_current_address;

    public:
        InterpreterScope(Memory::address_t start_address);

        Variable* get_variable(const std::string &name);
        std::map<std::string, Variable> &ref_variables();
        // const std::map<std::string, Variable> &ref_variables();

        Memory::address_t start_address() const;
        Memory::address_t current_address() const;

        // TODOO: padding?
        void shift_current_address(Memory::address_t shift);
        // we shifting current address to the left
        // | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 10 |
        //                                            ^ start_address

        InterpreterScope next_scope() const;
    };

    class InterpreterScopeStack
    {
        std::vector<InterpreterScope> m_scopes;
    public:
        InterpreterScopeStack(Memory::address_t start_address);

        Variable* get_variable(const std::string &name);
        void push();
        void pop();
        InterpreterResult create_variable(const std::string &name, Type type, size_t size_bytes, bool is_constant);
        std::vector<InterpreterScope> &ref_scopes();
        // const std::vector<InterpreterScope> &ref_scopes() const { return m_scopes; }
    };
}
