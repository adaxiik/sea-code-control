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
        bool m_initialized;
        bool m_is_parameter;    // only for debug/export purposes, doesnt do anything useful rn
    public:
        Variable(Type type, Memory::address_t address, bool is_constant = false, bool is_parameter = false) 
        : m_type(type)
        , m_address(address)
        , m_is_constant(is_constant) 
        , m_initialized(false)
        , m_is_parameter(is_parameter) {}

        std::optional<Type::Value> get_value(const Memory& memory) const
        {
            if (not m_initialized)
                return std::nullopt;

            return memory.read_value(m_address, m_type);
        }

        template <typename T>
        bool set_value(Memory& memory, T value)
        {
            if (m_is_constant)
                return false;

            if (!memory.write<T>(m_address, value))
                return false;
                
            m_initialized = true;
            return true;
        }

        bool set_value(Memory& memory, Type::Value value)
        {
            #define CASE_SET_VALUE(KIND) case Type::PrimitiveType::KIND: \
                return set_value<Type::Primitive::KIND>(memory, std::get<Type::Primitive::KIND>(std::get<Type::PrimitiveValue>(value.value)));

            if (m_type.is_struct())
            {
                // TODOOO: implement struct variables
                std::cerr << "Struct variables not implemented yet " << __FILE__ << ":" << __LINE__ << std::endl;
                std::abort();
            }

            if (m_type.is_pointer())
                return set_value<Type::Primitive::U64>(memory, std::get<Type::Primitive::U64>(std::get<Type::PrimitiveValue>(value.value)));

            switch (std::get<Type::PrimitiveType>(m_type.base_type))
            {
                CASE_SET_VALUE(Char)
                CASE_SET_VALUE(I8)
                CASE_SET_VALUE(U8)
                CASE_SET_VALUE(I16)
                CASE_SET_VALUE(U16)
                CASE_SET_VALUE(I32)
                CASE_SET_VALUE(U32)
                CASE_SET_VALUE(I64)
                CASE_SET_VALUE(U64)
                CASE_SET_VALUE(F32)
                CASE_SET_VALUE(F64)
                CASE_SET_VALUE(Bool)

                case Type::PrimitiveType::COUNT:
                default:
                    return false;
            }
        }

        Type type() const { return m_type; }
        Memory::address_t address() const { return m_address; }
        bool is_constant() const { return m_is_constant; }
        bool is_initialized() const { return m_initialized; }
        bool is_parameter() const { return m_is_parameter; }
    };  
} 
