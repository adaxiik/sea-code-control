#include "global_scope.hpp"

namespace scc
{
    GlobalScope::GlobalScope(Memory::address_t start_address)
    : m_scope(InterpreterScope(start_address)) {}

    InterpreterResult GlobalScope::create_variable(const std::string &name, Type type, size_t size_bytes, bool is_constant)
    {
        Variable* var{m_scope.get_from_scope(name)};

        if (var != nullptr)
            return InterpreterError::VariableAlreadyExistsError;
        
        m_scope.shift_current_address(size_bytes);

        Memory::address_t current_address = m_scope.current_address();
        m_scope.ref_scopes()
               .emplace(name,Variable{type, current_address, is_constant});
        
        type.modifiers.push_back(Type::Pointer{});

        return InterpreterResult::ok(InterpreterResultValue(current_address, type));
    }

    Variable *GlobalScope::get_variable(const std::string &name)
    {
        return m_scope.get_from_scope(name);
    }
}