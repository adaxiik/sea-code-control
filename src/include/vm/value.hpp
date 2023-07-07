#pragma once
#include <array>
#include <cstring>

namespace scc
{
    namespace vm
    {
        enum class ValueType
        {
            I32,
            I64,
            F32,
            F64,
            U32,
            U64,
            CHAR,
            BOOL,
        };


        class Value
        {
            std::array<char, 8> m_data;            
            static_assert(sizeof(m_data) == 8, "Value must be 8 bytes");

            ValueType m_type;

            template <typename T>
            ValueType determine_type()
            {
                if constexpr (std::is_same_v<T, int32_t>)
                    return ValueType::I32;
                else if constexpr (std::is_same_v<T, int64_t>)
                    return ValueType::I64;
                else if constexpr (std::is_same_v<T, float>)
                    return ValueType::F32;
                else if constexpr (std::is_same_v<T, double>)
                    return ValueType::F64;
                else if constexpr (std::is_same_v<T, uint32_t>)
                    return ValueType::U32;
                else if constexpr (std::is_same_v<T, uint64_t>)
                    return ValueType::U64;
                else if constexpr (std::is_same_v<T, char>)
                    return ValueType::CHAR;
                else if constexpr (std::is_same_v<T, bool>)
                    return ValueType::BOOL;
                else
                    static_assert(!sizeof(T), "Unknown type");


            }

        public:
            template <typename T>
            /* explicit */ constexpr Value(T value) : m_type(determine_type<T>())
            {
                static_assert(sizeof(T) <= sizeof(m_data), "Value must be 8 bytes");
                std::memcpy(m_data.data(), &value, sizeof(T));
            }

            template <typename T>
            constexpr Value(T value, ValueType type) : m_type(type)
            {
                static_assert(sizeof(T) <= sizeof(m_data), "Value must be 8 bytes");
                std::memcpy(m_data.data(), &value, sizeof(T));
            }

            template <typename T>
            constexpr T as()
            {
                static_assert(sizeof(T) <= sizeof(m_data), "Value must be 8 bytes");
                T value;
                std::memcpy(&value, m_data.data(), sizeof(T));
                return value;
            }

            template <typename T>
            constexpr operator T()
            {
                return as<T>();
            }

            constexpr Value() : m_data{0}, m_type(ValueType::I32) {}


        };     
    }
}