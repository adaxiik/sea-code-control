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
                return type_index == other.type_index and name == other.name and offset_bytes == other.offset_bytes;
            }

            bool operator<(const StructField& other) const
            {
                return type_index < other.type_index or name < other.name or offset_bytes < other.offset_bytes;
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

            bool operator<(const StructType& other) const
            {
                return name < other.name or fields < other.fields or size_bytes < other.size_bytes;
            }
        };

        struct ComparablePrimitive{
            bool operator==(const ComparablePrimitive&) const { return true; }
            bool operator<(const ComparablePrimitive&) const { return true; }
        };

        struct VoidType: public ComparablePrimitive
        {
            static constexpr auto *name = "void";
            static constexpr auto size_bytes = 0;
        };
        struct BoolType: public ComparablePrimitive
        {
            static constexpr auto *name = "bool";
            static constexpr auto size_bytes = 1;
        };
        struct CharType: public ComparablePrimitive
        {
            static constexpr auto *name = "char";
            static constexpr auto size_bytes = 1;
        };
        struct I8Type: public ComparablePrimitive
        {
            static constexpr auto *name = "i8";
            static constexpr auto size_bytes = 1;
        };
        struct I16Type: public ComparablePrimitive
        {
            static constexpr auto *name = "i16";
            static constexpr auto size_bytes = 2;
        };
        struct I32Type: public ComparablePrimitive
        {
            static constexpr auto *name = "i32";
            static constexpr auto size_bytes = 4;
        };
        struct I64Type: public ComparablePrimitive
        {
            static constexpr auto *name = "i64";
            static constexpr auto size_bytes = 8;
        };
        struct U8Type: public ComparablePrimitive
        {
            static constexpr auto *name = "u8";
            static constexpr auto size_bytes = 1;
        };
        struct U16Type: public ComparablePrimitive
        {
            static constexpr auto *name = "u16";
            static constexpr auto size_bytes = 2;
        };
        struct U32Type: public ComparablePrimitive
        {
            static constexpr auto *name = "u32";
            static constexpr auto size_bytes = 4;
        };
        struct U64Type: public ComparablePrimitive
        {
            static constexpr auto *name = "u64";
            static constexpr auto size_bytes = 8;
        };
        struct F32Type: public ComparablePrimitive
        {
            static constexpr auto *name = "f32";
            static constexpr auto size_bytes = 4;
        };
        struct F64Type: public ComparablePrimitive
        {
            static constexpr auto *name = "f64";
            static constexpr auto size_bytes = 8;
        };

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

            bool operator<(const PointerType& other) const
            {
                return pointing_to_type_index < other.pointing_to_type_index;
            }
        };

        struct ArrayType
        {
            TypeIndex element_type_index;
            uint64_t size_bytes;

            bool operator==(const ArrayType& other) const
            {
                return element_type_index == other.element_type_index and size_bytes == other.size_bytes;
            }

            bool operator<(const ArrayType& other) const
            {
                return element_type_index < other.element_type_index or size_bytes < other.size_bytes;
            }

        }; 

        using Type = std::variant<PrimitiveType, StructType, PointerType, ArrayType>;

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
            bool is_initialized;
        };

        struct Stackframe
        {
            std::vector<Variable> variables;
            std::vector<Variable> parameters;
        };

        struct ProgramSnapshot
        {
            std::vector<Type> types;
            std::vector<Variable> global_variables;
            std::vector<Stackframe> stackframes;
            std::vector<AllocatedPlace> anonymous_allocations;
        };

        ProgramSnapshot make_snapshot(const InterpreterState& state);
        std::string to_json(const ProgramSnapshot& snapshot);
    }
}