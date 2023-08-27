#pragma once
#include "scope.hpp"
#include "interpreter_result.hpp"
#include "memory.hpp"
#include "variable.hpp"

namespace scc
{
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