#include "interpreter_scope.hpp"

namespace  scc
{
    InterpreterScope::InterpreterScope(Memory::address_t start_address) 
                                     : m_start_address(start_address)
                                     , m_current_address(start_address) {}



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

    

    InterpreterScopeStack::InterpreterScopeStack(Memory::address_t start_address)
    {        
        m_scopes.emplace_back(InterpreterScope(start_address));
    }


    InterpreterResult InterpreterScopeStack::create_variable(const std::string &name, Type type, size_t size_bytes, bool is_constant = false)
    {
        if (m_scopes.empty()) // This should be unreachable, but you never know
            return InterpreterError::RuntimeError;

        Variable* var{m_scopes.back().get_from_scope(name)};

        if (var != nullptr)
            return InterpreterError::VariableAlreadyExistsError;
        
        m_scopes.back().shift_current_address(size_bytes);

        Memory::address_t current_address = m_scopes.back().current_address();
        m_scopes.back()
                .ref_scopes()
                .emplace(name,Variable{type, current_address, is_constant});
        

        return InterpreterResult::ok(static_cast<unsigned long long>(current_address));
    }

    std::vector<InterpreterScope> &InterpreterScopeStack::ref_scopes() { return m_scopes; }

    void InterpreterScopeStack::push() 
    { 
        m_scopes.emplace_back(m_scopes.back().next_scope()); 
    }
}
