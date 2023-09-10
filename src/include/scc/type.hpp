#pragma once
#include <variant>
#include <memory>
#include <iostream>
#include <optional>
#include "operation_result.hpp"
#include "overloaded.hpp"
namespace scc
{
    struct Type
    {
        struct Primitive
        {
            using Char = char;
            using I8 = signed char;
            using U8 = unsigned char;
            using I16 = signed short;
            using U16 = unsigned short;
            using I32 = signed int;
            using U32 = unsigned int;
            using I64 = signed long long;
            using U64 = unsigned long long;
            using F32 = float;
            using F64 = double;
            using Bool = bool;
            using Void = void;
        };

        using Value = std::variant<
            Primitive::Char,
            Primitive::I8,
            Primitive::U8,
            Primitive::I16,
            Primitive::U16,
            Primitive::I32,
            Primitive::U32,
            Primitive::I64,
            Primitive::U64,
            Primitive::F32,
            Primitive::F64,
            Primitive::Bool>;

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
            Void,
            COUNT
        };

        constexpr static Type from_value(const Value &value)
        {
            return std::visit(
                overloaded{
                    [](Primitive::Char) { return Type(Kind::Char); },
                    [](Primitive::I8) { return Type(Kind::I8); },
                    [](Primitive::U8) { return Type(Kind::U8); },
                    [](Primitive::I16) { return Type(Kind::I16); },
                    [](Primitive::U16) { return Type(Kind::U16); },
                    [](Primitive::I32) { return Type(Kind::I32); },
                    [](Primitive::U32) { return Type(Kind::U32); },
                    [](Primitive::I64) { return Type(Kind::I64); },
                    [](Primitive::U64) { return Type(Kind::U64); },
                    [](Primitive::F32) { return Type(Kind::F32); },
                    [](Primitive::F64) { return Type(Kind::F64); },
                    [](Primitive::Bool) { return Type(Kind::Bool); }},
                value);
        }

        Kind kind;
        uint32_t pointer_depth; // 0 = not a pointer, 1 = pointer to <kind>, 2 = pointer to pointer to <kind>, etc.
        // I hope no one needs more than 4 billion levels of indirection :D

        constexpr Type(Kind kind) : kind(kind), pointer_depth(0) {}
        constexpr Type(Kind kind, uint32_t pointer_depth) : kind(kind), pointer_depth(pointer_depth) {}
        constexpr Type(Kind kind, bool is_pointer) : kind(kind), pointer_depth(static_cast<uint32_t>(is_pointer)) {}

        constexpr size_t size_bytes() const
        {
            static_assert(sizeof(unsigned long long) == 8, "only 64-bit is supported");

            if (pointer_depth > 0)
                return sizeof(unsigned long long);

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
            case Kind::Void:
                return 0; // well its error most likely, but // TODOO:
            default:
                std::cerr << "UNREACHABLE at " << __FILE__ << ":" << __LINE__ << std::endl;
                std::exit(1);
                return 0;
            }
        }

        friend std::ostream &operator<<(std::ostream &os, const Type &type)
        {

            static_assert(static_cast<size_t>(Kind::COUNT) == 13, "Update this code");
            switch (type.kind)
            {
            case Kind::Char:
                os << "char";
                break;
            case Kind::I8:
                os << "signed char";
                break;
            case Kind::U8:
                os << "unsigned char";
                break;
            case Kind::I16:
                os << "signed short";
                break;
            case Kind::U16:
                os << "unsigned short";
                break;
            case Kind::I32:
                os << "signed int";
                break;
            case Kind::U32:
                os << "unsigned int";
                break;
            case Kind::I64:
                os << "signed long";
                break;
            case Kind::U64:
                os << "unsigned long";
                break;
            case Kind::F32:
                os << "float";
                break;
            case Kind::F64:
                os << "double";
                break;
            case Kind::Bool:
                os << "bool";
                break;
            case Kind::Void:
                os << "void";
                break;
            default:
                std::cerr << "Unknown type kind: " << static_cast<uint32_t>(type.kind) << std::endl;
                std::cerr << "UNREACHABLE at " << __FILE__ << ":" << __LINE__ << std::endl;
                std::exit(1);
            };

            for (uint32_t i = 0; i < type.pointer_depth; i++)
                os << "*";
            return os;
        }

        bool operator==(const Type &other) const
        {
            return kind == other.kind && pointer_depth == other.pointer_depth;
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
            else if constexpr (std::is_same_v<T, void>)
                return Type(Kind::Void);
            else if constexpr (std::is_same_v<T, OperationResult::InvalidOperation>)
            {
                std::cerr << "UNREACHABLE at " << __FILE__ << ":" << __LINE__ << std::endl;
                std::exit(1);
            }
            else
                static_assert(!sizeof(T), "Invalid type");

            return Type(Kind::COUNT);
        }

        template <typename T>
        static constexpr Type deduce_type(T)
        {
            return deduce_type<T>();
        }

        static std::optional<Type> from_string(const std::string &str)
        {
            if (str == "char")
                return Type(Kind::Char);
            else if (str == "signed char")
                return Type(Kind::I8);
            else if (str == "unsigned char")
                return Type(Kind::U8);
            else if (str == "signed short")
                return Type(Kind::I16);
            else if (str == "unsigned short")
                return Type(Kind::U16);
            else if (str == "signed int")
                return Type(Kind::I32);
            else if (str == "unsigned int")
                return Type(Kind::U32);
            else if (str == "signed long")
                return Type(Kind::I64);
            else if (str == "unsigned long")
                return Type(Kind::U64);
            else if (str == "float")
                return Type(Kind::F32);
            else if (str == "double")
                return Type(Kind::F64);
            // TODOOOO: change to _Bool and check it in bindings.. its probably not parsed as primitive type
            else if (str == "bool")
                return Type(Kind::Bool);
            else if (str == "int")
                return Type(Kind::I32);
            else if (str == "long")
                return Type(Kind::I64);
            else if (str == "short")
                return Type(Kind::I16);
            else if (str == "unsigned")
                return Type(Kind::U32);
            else if (str == "signed")
                return Type(Kind::I32);
            else if (str == "void")
                return Type(Kind::Void);
            else
                return std::nullopt;
        }

        bool is_pointer() const
        {
            return pointer_depth > 0;
        }
    };

}