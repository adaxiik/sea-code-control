#pragma once
#include <string>
#include <optional>
#include "type.hpp"
#include "memory.hpp"
namespace scc
{
    class Variable
    {
        Type m_type;
        Memory::address_t m_address;
        bool m_is_constant;
        // bool m_initialized;
        bool m_is_parameter;    // only for debug/export purposes, doesnt do anything useful rn
    public:
        Variable(Type type, Memory::address_t address, bool is_constant = false, bool is_parameter = false)
        : m_type(type)
        , m_address(address)
        , m_is_constant(is_constant)
        // , m_initialized(is_initialized)
        , m_is_parameter(is_parameter)
        {
            // int a[1]; a; is valid
            // I'll probably remove m_initialized soon
            // if (m_type.is_array() or m_type.is_struct())
            //     m_initialized = true;
            // update.. already removed
        }

        std::optional<Type::Value> get_value(const Memory& memory) const
        {
            // if (not m_initialized)
            //     return std::nullopt;

            // int a[5];
            // a == &a
            if (m_type.is_array() and not m_is_parameter)
                return Type::Value(static_cast<Type::Primitive::PTR>(m_address));

            return memory.read_value(m_address, m_type);
        }

        bool set_value(Memory& memory, Type::Value value, bool first_initilization = false)
        {
            if (m_is_constant and not first_initilization and not m_type.is_pointer())
                return false;

            if (not memory.write_value(m_address, value, m_type))
                return false;

            // m_initialized = true;
            return true;
        }

        Type type() const { return m_type; }
        Memory::address_t address() const { return m_address; }
        bool is_constant() const { return m_is_constant; }
        // bool is_initialized() const { return m_initialized; }
        bool is_parameter() const { return m_is_parameter; }
    };
}
