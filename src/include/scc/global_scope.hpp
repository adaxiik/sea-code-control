#pragma once
#include "interpreter_result.hpp"
#include "interpreter_scope.hpp"

namespace scc
{
    class GlobalScope
    {
        InterpreterScope m_scope;

    public:
        GlobalScope(Memory::address_t start_address);
        InterpreterResult create_variable(const std::string &name, Type type, size_t size_bytes, bool is_constant);
        Variable *get_variable(const std::string &name);
    };
}