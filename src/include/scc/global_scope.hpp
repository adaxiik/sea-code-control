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

        /**
         * @brief Creates a variable in the global scope
         *
         * @warning Creating a variable may invalidate all pointers obtained via get_variable
         *
         * @param name
         * @param type
         * @param size_bytes
         * @param is_constant
         * @return InterpreterResult
         */
        InterpreterResult create_variable(const std::string &name, Type type, size_t size_bytes, bool is_constant);
        Variable *get_variable(const std::string &name);

        auto begin() const { return m_scope.begin(); }
        auto end() const { return m_scope.end(); }
    };
}