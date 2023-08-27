#include "scope.hpp"

namespace  scc
{
    InterpreterScope::InterpreterScope(Memory::address_t start_address) 
                                     : m_start_address(start_address)
                                     , m_current_address(start_address) {}

    Variable* InterpreterScope::get_variable(const std::string &name)
    {
        auto var = m_variables.find(name);
        if (var != m_variables.end())
            return &var->second;
        return nullptr;
    }
    
    std::map<std::string, Variable> &InterpreterScope::ref_variables() 
    { 
        return m_variables; 
    }

    // const std::map<std::string, Variable> &InterpreterScope::ref_variables() const 
    // { 
    //     return m_variables; 
    // }

    Memory::address_t InterpreterScope::start_address() const 
    { 
        return m_start_address; 
    }

    Memory::address_t InterpreterScope::current_address() const 
    { 
        return m_current_address; 
    }

    void InterpreterScope::shift_current_address(Memory::address_t shift) 
    { 
        m_current_address -= shift; 
    }

    InterpreterScope InterpreterScope::next_scope() const 
    { 
        return InterpreterScope(m_current_address); 
    }

    
    //InterpreterScopeStack


    // InterpreterScopeStack(Memory::address_t start_address) : m_scopes{InterpreterScope(start_address)} {}

    InterpreterScopeStack::InterpreterScopeStack(Memory::address_t start_address) 
                        : m_scopes{InterpreterScope(start_address)} {}

    // Variable* get_variable(const std::string &name)
    // {
    //     for (auto it = m_scopes.rbegin(); it != m_scopes.rend(); it++)
    //     {
    //         auto var = it->get_variable(name);
    //         if (var != nullptr)
    //             return var;
    //     }

    //     return nullptr;
    // }

    Variable* InterpreterScopeStack::get_variable(const std::string &name)
    {
        for (auto it = m_scopes.rbegin(); it != m_scopes.rend(); it++)
        {
            auto var = it->get_variable(name);
            if (var != nullptr)
                return var;
        }

        return nullptr;
    }

    void InterpreterScopeStack::push() 
    {
        m_scopes.push_back(m_scopes.back().next_scope());
    }

    void InterpreterScopeStack::pop() 
    { 
        if (m_scopes.size() == 1)
            return;

        m_scopes.pop_back(); 
    }
    InterpreterResult InterpreterScopeStack::create_variable(const std::string &name, Type type, size_t size_bytes, bool is_constant = false)
    {
        if (m_scopes.empty()) // This should be unreachable, but you never know
            return InterpreterError::RuntimeError;

        Variable* var{m_scopes.back().get_variable(name)};

        if (var != nullptr)
            return InterpreterError::VariableAlreadyExistsError;
        
        m_scopes.back().shift_current_address(size_bytes);

        Memory::address_t current_address = m_scopes.back().current_address();
        m_scopes.back()
                .ref_variables()
                .emplace(name,Variable{type, current_address, is_constant});
        

        return InterpreterResult::ok(static_cast<unsigned long long>(current_address));
    }

    std::vector<InterpreterScope> &InterpreterScopeStack::ref_scopes() { return m_scopes; }
    // const std::vector<InterpreterScope> &InterpreterScopeStack::ref_scopes() const { return m_scopes; }
}
