#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <array>
#include "memory.hpp"
#include "interpreter_state.hpp"

namespace scc
{
    namespace export_format
    {
        using TypeIndex = uint32_t;
        using Byte = uint8_t;

        struct StructField
        {
            uint32_t offset_bytes;
            TypeIndex type_index;
            std::string name;

            bool operator==(const StructField& other) const
            {
                return offset_bytes == other.offset_bytes and type_index == other.type_index and name == other.name;
            }
        };

        struct StructType
        {
            std::string name;
            std::vector<StructField> fields;
            uint64_t size_bytes;

            bool operator==(const StructType& other) const
            {
                return name == other.name and fields == other.fields and size_bytes == other.size_bytes;
            }
        };


        #define SCC_EQUALITY_OPERATOR_FOR_PRIMITIVE_TYPE(TYPE) \
            bool operator==(const TYPE&) const \
            { \
                return true; \
            }

        #define SCC_PRIMITIVE_TYPE(TYPE, NAME, SIZE_BYTES) \
            struct TYPE \
            { \
                static constexpr auto *name = NAME; \
                static constexpr auto size_bytes = SIZE_BYTES; \
                SCC_EQUALITY_OPERATOR_FOR_PRIMITIVE_TYPE(TYPE) \
            };

        SCC_PRIMITIVE_TYPE(VoidType, "void", 0)
        SCC_PRIMITIVE_TYPE(BoolType, "bool", 1)
        SCC_PRIMITIVE_TYPE(CharType, "char", 1)
        SCC_PRIMITIVE_TYPE(I8Type, "signed char", 1)
        SCC_PRIMITIVE_TYPE(I16Type, "short", 2)
        SCC_PRIMITIVE_TYPE(I32Type, "int", 4)
        SCC_PRIMITIVE_TYPE(I64Type, "long", 8)
        SCC_PRIMITIVE_TYPE(U8Type, "unsigned char", 1)
        SCC_PRIMITIVE_TYPE(U16Type, "unsigned short", 2)
        SCC_PRIMITIVE_TYPE(U32Type, "unsigned int", 4)
        SCC_PRIMITIVE_TYPE(U64Type, "unsigned long", 8)
        SCC_PRIMITIVE_TYPE(F32Type, "float", 4)
        SCC_PRIMITIVE_TYPE(F64Type, "double", 8)


        using PrimitiveType = std::variant<
            VoidType,
            BoolType,
            CharType,
            I8Type,
            I16Type,
            I32Type,
            I64Type,
            U8Type,
            U16Type,
            U32Type,
            U64Type,
            F32Type,
            F64Type>;

        struct PointerType
        {
            TypeIndex pointing_to_type_index;
            static constexpr auto size_bytes = 8;

            bool operator==(const PointerType& other) const
            {
                return pointing_to_type_index == other.pointing_to_type_index;
            }
        };

        struct ArrayType
        {
            TypeIndex element_type_index;
            uint64_t size_bytes;
            uint64_t element_count;

            bool operator==(const ArrayType& other) const
            {
                return element_type_index == other.element_type_index
                    and size_bytes == other.size_bytes
                    and element_count == other.element_count;
            }
        };

        struct AliasType
        {
            std::string alias;
            TypeIndex aliased_type_index;
            uint64_t size_bytes;
        };

        using Type = std::variant<PrimitiveType, StructType, PointerType, ArrayType, AliasType>;

        struct AllocatedPlace
        {
            Memory::address_t address;
            uint64_t size_bytes;
            std::vector<Byte> data;
        };

        struct Variable
        {
            AllocatedPlace allocated_place;
            TypeIndex type_index;
            std::string name;
            // bool is_initialized;
        };

        struct Stackframe
        {
            std::string function_name;
            std::vector<Variable> variables;
            std::vector<Variable> parameters;
        };

        struct ProgramSnapshot
        {
            std::vector<Type> types;
            std::vector<Variable> global_variables;
            std::vector<Stackframe> stackframes;
            std::vector<AllocatedPlace> anonymous_allocations;
            std::vector<AllocatedPlace> read_only_allocations;
        };

        ProgramSnapshot make_snapshot(const InterpreterState& state);
        std::string to_json(const ProgramSnapshot& snapshot);
    }
}

template <>
struct std::hash<scc::export_format::StructField>
{
    size_t operator()(const scc::export_format::StructField& field) const
    {
        return std::hash<uint32_t>()(field.offset_bytes) ^ std::hash<scc::export_format::TypeIndex>()(field.type_index) ^ std::hash<std::string>()(field.name);
    }
};

template <>
struct std::hash<scc::export_format::StructType>
{
    size_t operator()(const scc::export_format::StructType& type) const
    {
        size_t vector_hash = 0;
        for (const auto& field : type.fields)
            vector_hash ^= std::hash<scc::export_format::StructField>()(field);

        return std::hash<std::string>()(type.name) ^ vector_hash ^ std::hash<uint64_t>()(type.size_bytes);
    }
};

template <>
struct std::hash<scc::export_format::PrimitiveType>
{
    size_t operator()(const scc::export_format::PrimitiveType& type) const
    {
        return std::visit(overloaded{
            [](const scc::export_format::VoidType) { return std::hash<std::string>()(scc::export_format::VoidType::name); },
            [](const scc::export_format::BoolType) { return std::hash<std::string>()(scc::export_format::BoolType::name); },
            [](const scc::export_format::CharType) { return std::hash<std::string>()(scc::export_format::CharType::name); },
            [](const scc::export_format::I8Type) { return std::hash<std::string>()(scc::export_format::I8Type::name); },
            [](const scc::export_format::I16Type) { return std::hash<std::string>()(scc::export_format::I16Type::name); },
            [](const scc::export_format::I32Type) { return std::hash<std::string>()(scc::export_format::I32Type::name); },
            [](const scc::export_format::I64Type) { return std::hash<std::string>()(scc::export_format::I64Type::name); },
            [](const scc::export_format::U8Type) { return std::hash<std::string>()(scc::export_format::U8Type::name); },
            [](const scc::export_format::U16Type) { return std::hash<std::string>()(scc::export_format::U16Type::name); },
            [](const scc::export_format::U32Type) { return std::hash<std::string>()(scc::export_format::U32Type::name); },
            [](const scc::export_format::U64Type) { return std::hash<std::string>()(scc::export_format::U64Type::name); },
            [](const scc::export_format::F32Type) { return std::hash<std::string>()(scc::export_format::F32Type::name); },
            [](const scc::export_format::F64Type) { return std::hash<std::string>()(scc::export_format::F64Type::name); },
        }, type);
    }
};

template <>
struct std::hash<scc::export_format::PointerType>
{
    size_t operator()(const scc::export_format::PointerType& type) const
    {
        return std::hash<scc::export_format::TypeIndex>()(type.pointing_to_type_index);
    }
};

template <>
struct std::hash<scc::export_format::ArrayType>
{
    size_t operator()(const scc::export_format::ArrayType& type) const
    {
        return std::hash<scc::export_format::TypeIndex>()(type.element_type_index) ^ std::hash<uint64_t>()(type.size_bytes) ^ std::hash<uint64_t>()(type.element_count);
    }
};

template <>
struct std::hash<scc::export_format::Type>
{
    size_t operator()(const scc::export_format::Type& type) const
    {
        return std::visit(overloaded{
            [](const scc::export_format::PrimitiveType& primitive_type) { return std::hash<scc::export_format::PrimitiveType>()(primitive_type); },
            [](const scc::export_format::StructType& struct_type) { return std::hash<scc::export_format::StructType>()(struct_type); },
            [](const scc::export_format::ArrayType& array_type) { return std::hash<scc::export_format::ArrayType>()(array_type); },
            [](const scc::export_format::PointerType& pointer_type) { return std::hash<scc::export_format::PointerType>()(pointer_type); },
            [](const scc::export_format::AliasType& alias_type) { return std::hash<std::string>()(alias_type.alias) ^ std::hash<scc::export_format::TypeIndex>()(alias_type.aliased_type_index) ^ std::hash<uint64_t>()(alias_type.size_bytes); },
        }, type);
    }
};
