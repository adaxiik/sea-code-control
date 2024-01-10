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
    // struct Type
    // {
    //     struct Primitive
    //     {
    //         using Char = char;
    //         using I8 = signed char;
    //         using U8 = unsigned char;
    //         using I16 = signed short;
    //         using U16 = unsigned short;
    //         using I32 = signed int;
    //         using U32 = unsigned int;
    //         using I64 = signed long long;
    //         using U64 = unsigned long long;
    //         using F32 = float;
    //         using F64 = double;
    //         using Bool = bool;
    //         using Void = void;
    //     };

    //     using Value = std::variant<
    //         Primitive::Char,
    //         Primitive::I8,
    //         Primitive::U8,
    //         Primitive::I16,
    //         Primitive::U16,
    //         Primitive::I32,
    //         Primitive::U32,
    //         Primitive::I64,
    //         Primitive::U64,
    //         Primitive::F32,
    //         Primitive::F64,
    //         Primitive::Bool>;

    //     enum class Kind
    //     {
    //         Char = 0,
    //         I8,
    //         U8,
    //         I16,
    //         U16,
    //         I32,
    //         U32,
    //         I64,
    //         U64,
    //         F32,
    //         F64,
    //         Bool,
    //         Void,
    //         COUNT
    //     };

    //     constexpr static Type from_value(const Value &value)
    //     {
    //         return std::visit(
    //             overloaded{
    //                 [](Primitive::Char) { return Type(Kind::Char); },
    //                 [](Primitive::I8) { return Type(Kind::I8); },
    //                 [](Primitive::U8) { return Type(Kind::U8); },
    //                 [](Primitive::I16) { return Type(Kind::I16); },
    //                 [](Primitive::U16) { return Type(Kind::U16); },
    //                 [](Primitive::I32) { return Type(Kind::I32); },
    //                 [](Primitive::U32) { return Type(Kind::U32); },
    //                 [](Primitive::I64) { return Type(Kind::I64); },
    //                 [](Primitive::U64) { return Type(Kind::U64); },
    //                 [](Primitive::F32) { return Type(Kind::F32); },
    //                 [](Primitive::F64) { return Type(Kind::F64); },
    //                 [](Primitive::Bool) { return Type(Kind::Bool); }},
    //             value);
    //     }

    //     Kind kind;
    //     uint32_t pointer_depth; // 0 = not a pointer, 1 = pointer to <kind>, 2 = pointer to pointer to <kind>, etc.
    //     // I hope no one needs more than 4 billion levels of indirection :D

    //     constexpr Type(Kind kind) : kind(kind), pointer_depth(0) {}
    //     constexpr Type(Kind kind, uint32_t pointer_depth) : kind(kind), pointer_depth(pointer_depth) {}
    //     constexpr Type(Kind kind, bool is_pointer) : kind(kind), pointer_depth(static_cast<uint32_t>(is_pointer)) {}

    //     constexpr size_t size_bytes() const
    //     {
    //         static_assert(sizeof(unsigned long long) == 8, "only 64-bit is supported");

    //         if (pointer_depth > 0)
    //             return sizeof(unsigned long long);

    //         switch (kind)
    //         {
    //         case Kind::Char:
    //             return sizeof(char);
    //         case Kind::I8:
    //             return sizeof(signed char);
    //         case Kind::U8:
    //             return sizeof(unsigned char);
    //         case Kind::I16:
    //             return sizeof(signed short);
    //         case Kind::U16:
    //             return sizeof(unsigned short);
    //         case Kind::I32:
    //             return sizeof(signed int);
    //         case Kind::U32:
    //             return sizeof(unsigned int);
    //         case Kind::I64:
    //             return sizeof(signed long);
    //         case Kind::U64:
    //             return sizeof(unsigned long);
    //         case Kind::F32:
    //             return sizeof(float);
    //         case Kind::F64:
    //             return sizeof(double);
    //         case Kind::Bool:
    //             return sizeof(bool);
    //         case Kind::Void:
    //             return 0; // well its error most likely, but // TODOO:
    //         default:
    //             std::cerr << "UNREACHABLE at " << __FILE__ << ":" << __LINE__ << std::endl;
    //             std::exit(1);
    //             return 0;
    //         }
    //     }

    //     friend std::ostream &operator<<(std::ostream &os, const Type &type)
    //     {

    //         static_assert(static_cast<size_t>(Kind::COUNT) == 13, "Update this code");
    //         switch (type.kind)
    //         {
    //         case Kind::Char:
    //             os << "char";
    //             break;
    //         case Kind::I8:
    //             os << "signed char";
    //             break;
    //         case Kind::U8:
    //             os << "unsigned char";
    //             break;
    //         case Kind::I16:
    //             os << "signed short";
    //             break;
    //         case Kind::U16:
    //             os << "unsigned short";
    //             break;
    //         case Kind::I32:
    //             os << "signed int";
    //             break;
    //         case Kind::U32:
    //             os << "unsigned int";
    //             break;
    //         case Kind::I64:
    //             os << "signed long";
    //             break;
    //         case Kind::U64:
    //             os << "unsigned long";
    //             break;
    //         case Kind::F32:
    //             os << "float";
    //             break;
    //         case Kind::F64:
    //             os << "double";
    //             break;
    //         case Kind::Bool:
    //             os << "bool";
    //             break;
    //         case Kind::Void:
    //             os << "void";
    //             break;
    //         default:
    //             std::cerr << "Unknown type kind: " << static_cast<uint32_t>(type.kind) << std::endl;
    //             std::cerr << "UNREACHABLE at " << __FILE__ << ":" << __LINE__ << std::endl;
    //             std::exit(1);
    //         };

    //         for (uint32_t i = 0; i < type.pointer_depth; i++)
    //             os << "*";
    //         return os;
    //     }

    //     bool operator==(const Type &other) const
    //     {
    //         return kind == other.kind && pointer_depth == other.pointer_depth;
    //     }

    //     bool operator!=(const Type &other) const
    //     {
    //         return !(*this == other);
    //     }

    //     template <typename T>
    //     static constexpr Type deduce_type()
    //     {

    //         if constexpr (std::is_same_v<T, char>)
    //             return Type(Kind::Char);
    //         else if constexpr (std::is_same_v<T, signed char>)
    //             return Type(Kind::I8);
    //         else if constexpr (std::is_same_v<T, unsigned char>)
    //             return Type(Kind::U8);
    //         else if constexpr (std::is_same_v<T, short>)
    //             return Type(Kind::I16);
    //         else if constexpr (std::is_same_v<T, unsigned short>)
    //             return Type(Kind::U16);
    //         else if constexpr (std::is_same_v<T, int>)
    //             return Type(Kind::I32);
    //         else if constexpr (std::is_same_v<T, unsigned int>)
    //             return Type(Kind::U32);
    //         else if constexpr (std::is_same_v<T, long>)
    //             return Type(Kind::I64);
    //         else if constexpr (std::is_same_v<T, long long>)
    //             return Type(Kind::I64);
    //         else if constexpr (std::is_same_v<T, unsigned long>)
    //             return Type(Kind::U64);
    //         else if constexpr (std::is_same_v<T, unsigned long long>)
    //             return Type(Kind::U64);
    //         else if constexpr (std::is_same_v<T, float>)
    //             return Type(Kind::F32);
    //         else if constexpr (std::is_same_v<T, double>)
    //             return Type(Kind::F64);
    //         else if constexpr (std::is_same_v<T, bool>)
    //             return Type(Kind::Bool);
    //         else if constexpr (std::is_same_v<T, void>)
    //             return Type(Kind::Void);
    //         else if constexpr (std::is_same_v<T, OperationResult::InvalidOperation>)
    //         {
    //             std::cerr << "UNREACHABLE at " << __FILE__ << ":" << __LINE__ << std::endl;
    //             std::exit(1);
    //         }
    //         else
    //             static_assert(!sizeof(T), "Invalid type");

    //         return Type(Kind::COUNT);
    //     }

    //     template <typename T>
    //     static constexpr Type deduce_type(T)
    //     {
    //         return deduce_type<T>();
    //     }

    //     static std::optional<Type> from_string(const std::string &str)
    //     {
    //         if (str == "char")
    //             return Type(Kind::Char);
    //         else if (str == "signed char")
    //             return Type(Kind::I8);
    //         else if (str == "unsigned char")
    //             return Type(Kind::U8);
    //         else if (str == "signed short")
    //             return Type(Kind::I16);
    //         else if (str == "unsigned short")
    //             return Type(Kind::U16);
    //         else if (str == "signed int")
    //             return Type(Kind::I32);
    //         else if (str == "unsigned int")
    //             return Type(Kind::U32);
    //         else if (str == "signed long")
    //             return Type(Kind::I64);
    //         else if (str == "unsigned long")
    //             return Type(Kind::U64);
    //         else if (str == "float")
    //             return Type(Kind::F32);
    //         else if (str == "double")
    //             return Type(Kind::F64);
    //         // TODOOOO: change to _Bool and check it in bindings.. its probably not parsed as primitive type
    //         else if (str == "bool")
    //             return Type(Kind::Bool);
    //         else if (str == "int")
    //             return Type(Kind::I32);
    //         else if (str == "long")
    //             return Type(Kind::I64);
    //         else if (str == "short")
    //             return Type(Kind::I16);
    //         else if (str == "unsigned")
    //             return Type(Kind::U32);
    //         else if (str == "signed")
    //             return Type(Kind::I32);
    //         else if (str == "void")
    //             return Type(Kind::Void);
    //         else
    //             return std::nullopt;
    //     }

    //     bool is_pointer() const
    //     {
    //         return pointer_depth > 0;
    //     }
    // };

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
            std::string name;
            std::map<std::string, Type> fields;

            bool operator==(const StructType &other) const
            {
                return name == other.name;
                // TODO: maybe check fields too?
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

        constexpr Type(PrimitiveType primitive_type) : base_type(primitive_type) {}
        Type(StructType struct_type) : base_type(struct_type) {}
        Type(BaseType base_type, std::vector<std::variant<Pointer, Array>> modifiers) : base_type(base_type), modifiers(modifiers) {}

        constexpr size_t size_bytes() const
        {
            static_assert(sizeof(unsigned long long) == 8, "only 64-bit is supported");

            if (modifiers.size() > 0)
            {
                size_t accumulated_size = 1;
                for (size_t i = 0; i < modifiers.size(); i++)
                {
                    const bool is_last = i == modifiers.size() - 1;
                    const bool is_array = std::holds_alternative<Array>(modifiers[i]);

                    // last => array => accumulated * base type
                    // last => pointer => accumulated * sizeof(void*)
                    // not last => array => accumulated * array size
                    // not last => pointer => accumulated * sizeof(void*), break

                    if (not is_last and is_array)
                    {
                        accumulated_size *= std::get<Array>(modifiers[i]).size;
                    }
                    else if (not is_last)
                    {
                        accumulated_size *= sizeof(Primitive::PTR);
                        return accumulated_size;
                    }
                    else if (is_array)
                    {
                        return accumulated_size * std::get<Array>(modifiers[i]).size * std::visit(
                            overloaded{
                                [](PrimitiveType primitive_type) { return Type(primitive_type).size_bytes(); },
                                [](StructType ) { 
                                    std::cerr << "Struct variables not implemented yet " << __FILE__ << ":" << __LINE__ << std::endl;
                                    std::abort();
                                    return size_t(0);    
                                }
                            },
                            base_type);
                    }
                    else
                    {
                        return accumulated_size * sizeof(Primitive::PTR);
                    }
                }
                // unreachable
                std::cerr << "UNREACHABLE at " << __FILE__ << ":" << __LINE__ << std::endl;
                return accumulated_size;
            }

            if (std::holds_alternative<StructType>(base_type))
            {
                std::cerr << "Struct variables not implemented yet " << __FILE__ << ":" << __LINE__ << std::endl;
                std::abort();
            }

            switch (std::get<PrimitiveType>(base_type))
            {
            case PrimitiveType::Char:
                return sizeof(Primitive::Char);
            case PrimitiveType::I8:
                return sizeof(Primitive::I8);
            case PrimitiveType::U8:
                return sizeof(Primitive::U8);
            case PrimitiveType::I16:
                return sizeof(Primitive::I16);
            case PrimitiveType::U16:
                return sizeof(Primitive::U16);
            case PrimitiveType::I32:
                return sizeof(Primitive::I32);
            case PrimitiveType::U32:
                return sizeof(Primitive::U32);
            case PrimitiveType::I64:
                return sizeof(Primitive::I64);
            case PrimitiveType::U64:
                return sizeof(Primitive::U64);
            case PrimitiveType::F32:
                return sizeof(Primitive::F32);
            case PrimitiveType::F64:
                return sizeof(Primitive::F64);
            case PrimitiveType::Bool:
                return sizeof(Primitive::Bool);
            case PrimitiveType::Void:
                return 0; // well its error most likely, but // TODOO:
            default:
                std::cerr << "UNREACHABLE at " << __FILE__ << ":" << __LINE__ << std::endl;
                std::exit(1);
                return 0;
            }
        }

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
        
        struct StructValue {
            std::map<std::string, std::variant<PrimitiveValue, StructValue>> fields;            
        };

        struct Value;

        template <typename T>
        static constexpr Type deduce_type()
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
    };

}