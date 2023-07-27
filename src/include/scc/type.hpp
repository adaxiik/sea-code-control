#pragma once
#include <variant>
#include <memory>
#include <iostream>
#include <optional>
#include "operation_result.hpp"
namespace scc
{
    struct Type
    {
        enum class Kind
        {
            Char = 0,
            I8,
            U8,
            I16,
            U16,
            I32,
            U32,
            I64,
            U64,
            F32,
            F64,
            Bool,
            COUNT
        };

        Kind kind;

        constexpr Type(Kind kind) : kind(kind) {}

        constexpr size_t size_bytes() const
        {

            switch (kind)
            {
            case Kind::Char:
                return sizeof(char);
            case Kind::I8:
                return sizeof(signed char);
            case Kind::U8:
                return sizeof(unsigned char);
            case Kind::I16:
                return sizeof(signed short);
            case Kind::U16:
                return sizeof(unsigned short);
            case Kind::I32:
                return sizeof(signed int);
            case Kind::U32:
                return sizeof(unsigned int);
            case Kind::I64:
                return sizeof(signed long);
            case Kind::U64:
                return sizeof(unsigned long);
            case Kind::F32:
                return sizeof(float);
            case Kind::F64:
                return sizeof(double);
            case Kind::Bool:
                return sizeof(bool);
            default:
                std::cerr << "UNREACHABLE at " << __FILE__ << ":" << __LINE__ << std::endl;
                std::exit(1);
                return 0;
            }
        }

        friend std::ostream &operator<<(std::ostream &os, const Type &type)
        {

            static_assert(static_cast<size_t>(Kind::COUNT) == 12, "Update this code");
            switch (type.kind)
            {
            case Kind::Char: os << "char";           break;
            case Kind::I8:   os << "signed char";    break;
            case Kind::U8:   os << "unsigned char";  break;
            case Kind::I16:  os << "signed short";   break;
            case Kind::U16:  os << "unsigned short"; break;
            case Kind::I32:  os << "signed int";     break;
            case Kind::U32:  os << "unsigned int";   break;
            case Kind::I64:  os << "signed long";    break;
            case Kind::U64:  os << "unsigned long";  break;
            case Kind::F32:  os << "float";          break;
            case Kind::F64:  os << "double";         break;
            case Kind::Bool: os << "bool";           break;
            default:
                std::cerr << "UNREACHABLE at " << __FILE__ << ":" << __LINE__ << std::endl;
                std::exit(1);
            };

            return os;
        }

        bool operator==(const Type &other) const
        {
            return kind == other.kind;
        }

        bool operator!=(const Type &other) const
        {
            return !(*this == other);
        }


        template <typename T>
        static constexpr Type deduce_type()
        {
           
            if constexpr (std::is_same_v<T, char>)
                return Type(Kind::Char);
            else if constexpr (std::is_same_v<T, signed char>)
                return Type(Kind::I8);
            else if constexpr (std::is_same_v<T, unsigned char>)
                return Type(Kind::U8);
            else if constexpr (std::is_same_v<T, short>)
                return Type(Kind::I16);
            else if constexpr (std::is_same_v<T, unsigned short>)
                return Type(Kind::U16);
            else if constexpr (std::is_same_v<T, int>)
                return Type(Kind::I32);
            else if constexpr (std::is_same_v<T, unsigned int>)
                return Type(Kind::U32);
            else if constexpr (std::is_same_v<T, long>)
                return Type(Kind::I64);
            else if constexpr (std::is_same_v<T, long long>)
                return Type(Kind::I64);
            else if constexpr (std::is_same_v<T, unsigned long>)
                return Type(Kind::U64);
            else if constexpr (std::is_same_v<T, unsigned long long>)
                return Type(Kind::U64);
            else if constexpr (std::is_same_v<T, float>)
                return Type(Kind::F32);
            else if constexpr (std::is_same_v<T, double>)
                return Type(Kind::F64);
            else if constexpr (std::is_same_v<T, bool>)
                return Type(Kind::Bool);
            else if constexpr (std::is_same_v<T, OperationResult::InvalidOperation>)
            {
                std::cerr << "UNREACHABLE at " << __FILE__ << ":" << __LINE__ << std::endl;
                std::exit(1);
            }
            else
                static_assert(!sizeof(T), "Invalid type");

            return Type(Kind::COUNT);
        }
    };
}