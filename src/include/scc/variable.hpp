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
    public:
        Variable(Type type, Memory::address_t address, bool is_constant = false) 
        : m_type(type)
        , m_address(address)
        , m_is_constant(is_constant) 
        , m_initialized(false){}

        template <typename T>
        std::optional<T> get_value(const Memory& memory) const
        {
            if (!m_initialized)
                return std::nullopt;
            
            return memory.read<T>(m_address);
        }

        std::optional<Type::Value> get_value(const Memory& memory, Type type) const
        {
            #define CASE_GET_VALUE(KIND) case Type::Kind::KIND: \
                { \
                    auto value = get_value<Type::Primitive::KIND>(memory); \
                    if (!value.has_value()) \
                        return std::nullopt; \
                    return Type::Value(value.value()); \
                } \
                break;
            switch (type.kind)
            {
                CASE_GET_VALUE(Char)
                CASE_GET_VALUE(I8)
                CASE_GET_VALUE(U8)
                CASE_GET_VALUE(I16)
                CASE_GET_VALUE(U16)
                CASE_GET_VALUE(I32)
                CASE_GET_VALUE(U32)
                CASE_GET_VALUE(I64)
                CASE_GET_VALUE(U64)
                CASE_GET_VALUE(F32)
                CASE_GET_VALUE(F64)
                CASE_GET_VALUE(Bool)

                case Type::Kind::COUNT:
                default:
                    return std::nullopt;
            }
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
            #define CASE_SET_VALUE(KIND) case Type::Kind::KIND: \
                return set_value<Type::Primitive::KIND>(memory, std::get<Type::Primitive::KIND>(value));
            switch (m_type.kind)
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

                case Type::Kind::COUNT:
                default:
                    return false;
            }
        }

        Type type() const { return m_type; }
        Memory::address_t address() const { return m_address; }
        bool is_constant() const { return m_is_constant; }
        bool is_initialized() const { return m_initialized; }
    };  
} 
