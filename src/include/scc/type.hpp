#pragma once
#include <variant>
#include <memory>
#include <iostream>
#include <optional>
#include <vector>
#include <map>
#include <cstdint>
#include <algorithm>

#include "operation_result.hpp"
#include "overloaded.hpp"
namespace scc
{
    struct Type
    {
        // int x;
        // int *x;
        // int **x;
        // int x[10];  // array 10 of int
        // int *x[10]; // array 10 of pointer to int
        // int x[10][20]; // array 10 of array 20 of int
        // int (*x[10])[20]; // array 10 of pointer to array 20 of int

        enum class PrimitiveType
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

        struct StructType
        {
            using Field = std::pair<std::string, Type>;

            std::string name;
            // std::map<std::string, Type> fields;
            std::vector<Field> fields;

            bool operator==(const StructType &other) const
            {
                return name == other.name;
                // TODO: maybe check fields too?
            }

            std::optional<size_t> offset_of(const std::string &field_name) const
            {
                size_t offset = 0;
                for (const auto &[name, type] : fields)
                {
                    if (name == field_name)
                        return offset;
                    offset += type.size_bytes();
                }
                return std::nullopt;
            }

            std::optional<Type> type_of(const std::string &field_name) const
            {
                for (const auto &[name, type] : fields)
                {
                    if (name == field_name)
                        return type;
                }
                return std::nullopt;
            }
        };

        using BaseType = std::variant<PrimitiveType, StructType>;

        struct Pointer {};
        struct Array
        {
            uint64_t size;
        };

        BaseType base_type;

        using Modifier = std::variant<Pointer, Array>;
        // sadly, we dont have constexpr std::vector yet :(
        std::vector<Modifier> modifiers;

        // int x;   // {}
        // int *x;  // {Pointer}
        // int **x; // {Pointer, Pointer}
        // int x[10];  // {Array{10}}
        // int *x[10]; // {Array{10}, Pointer}
        // int x[10][20]; // {Array{10}, Array{20}}
        // int (*x[10])[20]; // {Array{10}, Pointer, Array{20}}

        Type(PrimitiveType primitive_type) : base_type(primitive_type) {}
        Type(StructType struct_type) : base_type(struct_type) {}
        Type(BaseType base_type, std::vector<std::variant<Pointer, Array>> modifiers) : base_type(base_type), modifiers(modifiers) {}
        size_t size_bytes() const;

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
            using PTR = U64;
            static_assert(sizeof(Char) == 1, "char is not 1 byte");
            static_assert(sizeof(I8) == 1, "signed char is not 1 byte");
            static_assert(sizeof(U8) == 1, "unsigned char is not 1 byte");
            static_assert(sizeof(I16) == 2, "signed short is not 2 bytes");
            static_assert(sizeof(U16) == 2, "unsigned short is not 2 bytes");
            static_assert(sizeof(I32) == 4, "signed int is not 4 bytes");
            static_assert(sizeof(U32) == 4, "unsigned int is not 4 bytes");
            static_assert(sizeof(I64) == 8, "signed long long is not 8 bytes");
            static_assert(sizeof(U64) == 8, "unsigned long long is not 8 bytes");
            static_assert(sizeof(F32) == 4, "float is not 4 bytes");
            static_assert(sizeof(F64) == 8, "double is not 8 bytes");
            static_assert(sizeof(Bool) == 1, "bool is not 1 byte");
        };
        using PrimitiveValue = std::variant<
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

        struct Value;
        struct StructValue {
            std::map<std::string, std::variant<PrimitiveValue, StructValue>> fields;
            // std::map<std::string, Value> fields;
        };


        template <typename T>
        static Type deduce_type()
        {
            if constexpr (std::is_same_v<T, char>)
                return Type(PrimitiveType::Char);
            else if constexpr (std::is_same_v<T, signed char>)
                return Type(PrimitiveType::I8);
            else if constexpr (std::is_same_v<T, unsigned char>)
                return Type(PrimitiveType::U8);
            else if constexpr (std::is_same_v<T, short>)
                return Type(PrimitiveType::I16);
            else if constexpr (std::is_same_v<T, unsigned short>)
                return Type(PrimitiveType::U16);
            else if constexpr (std::is_same_v<T, int>)
                return Type(PrimitiveType::I32);
            else if constexpr (std::is_same_v<T, unsigned int>)
                return Type(PrimitiveType::U32);
            else if constexpr (std::is_same_v<T, long>)
                return Type(PrimitiveType::I64);
            else if constexpr (std::is_same_v<T, long long>)
                return Type(PrimitiveType::I64);
            else if constexpr (std::is_same_v<T, unsigned long>)
                return Type(PrimitiveType::U64);
            else if constexpr (std::is_same_v<T, unsigned long long>)
                return Type(PrimitiveType::U64);
            else if constexpr (std::is_same_v<T, float>)
                return Type(PrimitiveType::F32);
            else if constexpr (std::is_same_v<T, double>)
                return Type(PrimitiveType::F64);
            else if constexpr (std::is_same_v<T, bool>)
                return Type(PrimitiveType::Bool);
            else if constexpr (std::is_same_v<T, void>)
                return Type(PrimitiveType::Void);
            else if constexpr (std::is_same_v<T, OperationResult::InvalidOperation>)
            {
                std::cerr << "UNREACHABLE at " << __FILE__ << ":" << __LINE__ << std::endl;
                std::exit(1);
            }
            else
                static_assert(!sizeof(T), "Invalid type");

            return Type(PrimitiveType::COUNT);
        }


        static Type from_value(const Value &value);
        static std::optional<Type> from_string(const std::string &str);

        bool operator==(const Type &other) const;
        bool operator!=(const Type &other) const;
        friend std::ostream &operator<<(std::ostream &os, const Type &type);
        bool is_pointer() const { return modifiers.size() > 0; }
        bool is_struct() const { return std::holds_alternative<StructType>(base_type); }
        bool is_primitive() const { return std::holds_alternative<PrimitiveType>(base_type); }
        bool is_array() const { return modifiers.size() > 0 and std::holds_alternative<Array>(modifiers.back()); }
        size_t pointer_depth() const { return modifiers.size(); }

        std::optional<PrimitiveType> primitive_type() const
        {
            if (is_primitive())
                return std::get<PrimitiveType>(base_type);
            else
                return std::nullopt;
        }
    };

    struct Type::Value {
        Type type;
        std::variant<PrimitiveValue, StructValue> value;

        Value(Type type, PrimitiveValue value) : type(type), value(value) {}
        Value(Type type, StructValue value) : type(type), value(value) {}

        template<typename T>
        Value(T value) : type(Type::deduce_type<T>()), value(value) {}

        template<typename T>
        static Value from_primitive(T value) {
            return Value{Type::deduce_type<T>(), PrimitiveValue(value)};
        }

        std::optional<PrimitiveValue> primitive_value() const {
            if (std::holds_alternative<PrimitiveValue>(value))
                return std::get<PrimitiveValue>(value);
            else
                return std::nullopt;
        }

        void print(std::ostream &os) const { print_as_type(os, value, type); }
        static void print_as_type(std::ostream &os, std::variant<PrimitiveValue, StructValue> value, const Type &type);
    };
}

template <>
struct std::hash<scc::Type>
{
    size_t operator()(const scc::Type &type) const;
};


template <>
struct std::hash<scc::Type::StructType>
{
    size_t operator()(const scc::Type::StructType &struct_type) const;
};

template <>
struct std::hash<scc::Type::BaseType>
{
    size_t operator()(const scc::Type::BaseType &base_type) const;
};

template <>
struct std::hash<scc::Type::Modifier>
{
    size_t operator()(const scc::Type::Modifier &modifier) const;
};
