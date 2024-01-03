#include "export.hpp"
#include "3rdparty/json.hpp"
#include "interpreter_state.hpp"
#include "overloaded.hpp"
#include "type.hpp"
#include <variant>

namespace scc::export_format
{

    struct GetNameOfPrimitiveType
    {
        template <typename T>
        std::string operator()(T) const{ return T::name; }
    };

    struct GetSizeOfType
    {
        template <typename T>
        uint64_t operator()(T) const{ return T::size_bytes; }
        uint64_t operator()(const StructType& struct_type) const{ return struct_type.size_bytes; }
        uint64_t operator()(const ArrayType& array_type) const{ return array_type.size_bytes; }
        uint64_t operator()(const PointerType& pointer_type) const{ return pointer_type.size_bytes; }
        uint64_t operator()(const PrimitiveType& primitive_type) const{ return std::visit(GetSizeOfType{}, primitive_type); }
    };

    static void to_json(nlohmann::json& j, const StructField& type)
    {
        j = nlohmann::json{
            {"type_index", type.type_index},
            {"name", type.name},
            {"offset_bytes", type.offset_bytes}
        };
    }

    static void to_json(nlohmann::json& j, const Type type)
    {
        j["size_bytes"] = std::visit(GetSizeOfType{}, type);

        j["kind"] = std::visit(overloaded{
            [](const PrimitiveType) { return "primitive"; },
            [](const StructType) { return "struct"; },
            [](const ArrayType) { return "array"; },
            [](const PointerType) { return "pointer"; },
        }, type);

        std::visit(overloaded{
            [&j](const PrimitiveType& primitive_type) { j["name"] = std::visit(GetNameOfPrimitiveType{}, primitive_type); },
            [&j](const StructType& struct_type) { j["name"] = struct_type.name; j["fields"] = struct_type.fields; },
            [&j](const ArrayType& array_type) { j["element_type_index"] = array_type.element_type_index; },
            [&j](const PointerType& pointer_type) { j["pointing_to_type_index"] = pointer_type.pointing_to_type_index; },
        }, type);
    }

    static void to_json(nlohmann::json& j, const AllocatedPlace& place)
    {
        j = nlohmann::json{{"address", place.address}, {"size_bytes", place.size_bytes}, {"data", place.data}};
    }

    static void to_json(nlohmann::json& j, const Variable& variable)
    {
        j = nlohmann::json{
            {"allocated_place", variable.allocated_place},
            {"type_index", variable.type_index},
            {"name", variable.name},
            // {"is_constant", variable.is_constant}, // TODO: do I care? 
            {"is_initialized", variable.is_initialized}
        };
    }

    static void to_json(nlohmann::json& j, const Stackframe& stackframe)
    {
        j = nlohmann::json{{"variables", stackframe.variables}, {"parameters", stackframe.parameters}};
    }

    std::string to_json(const ProgramSnapshot& snapshot)
    {
        return nlohmann::json {
            {"types", snapshot.types},
            {"global_variables", snapshot.global_variables},
            {"stackframes", snapshot.stackframes},
            {"anonymous_allocations", snapshot.anonymous_allocations}
        }.dump(4);
    }

    static void snapshot_type(ProgramSnapshot& snapshot, const scc::Type& type)
    {
        // TODOOOOOOOOOO: after having proper way of expressing struct, pointers and arrays with scc:Type, please fix this, and all bellow :)

    }

    static scc::export_format::PrimitiveType type_to_export_type(const scc::Type& type)
    {
        if (type.is_pointer())
        {
            std::cerr << "type_to_export_type: pointer type is not supported yet" << std::endl;
            std::abort();
        }

        static_assert(static_cast<int>(scc::Type::Kind::COUNT) == 13);
        switch (type.kind)
        {
        case scc::Type::Kind::Char: return scc::export_format::PrimitiveType{scc::export_format::CharType{}};
        case scc::Type::Kind::I8:   return scc::export_format::PrimitiveType{scc::export_format::I8Type{}};
        case scc::Type::Kind::I16:  return scc::export_format::PrimitiveType{scc::export_format::I16Type{}};
        case scc::Type::Kind::I32:  return scc::export_format::PrimitiveType{scc::export_format::I32Type{}};
        case scc::Type::Kind::I64:  return scc::export_format::PrimitiveType{scc::export_format::I64Type{}};
        case scc::Type::Kind::U8:   return scc::export_format::PrimitiveType{scc::export_format::U8Type{}};
        case scc::Type::Kind::U16:  return scc::export_format::PrimitiveType{scc::export_format::U16Type{}};
        case scc::Type::Kind::U32:  return scc::export_format::PrimitiveType{scc::export_format::U32Type{}};
        case scc::Type::Kind::U64:  return scc::export_format::PrimitiveType{scc::export_format::U64Type{}};
        case scc::Type::Kind::F32:  return scc::export_format::PrimitiveType{scc::export_format::F32Type{}};
        case scc::Type::Kind::F64:  return scc::export_format::PrimitiveType{scc::export_format::F64Type{}};
        case scc::Type::Kind::Bool: return scc::export_format::PrimitiveType{scc::export_format::BoolType{}};
        default:
            std::cerr << "type_to_export_type: unknown type kind: " << static_cast<int>(type.kind) << std::endl;
            std::abort();
            break;
        }
    }

    static void snapshot_global_variables(ProgramSnapshot& snapshot, const InterpreterState& state, std::function<TypeIndex(const Type&)> get_type_index_of)
    {
        for (const auto& [name, variable] : state.global_scope)
        {
            std::vector<Byte> data;

            if (variable.is_initialized())
            {
                data.resize(variable.type().size_bytes());
                state.memory.read_into(variable.address(), data.data(), data.size()); 
            }

            snapshot.global_variables.push_back({
                .allocated_place = {
                    .address = variable.address(),
                    .size_bytes = variable.type().size_bytes(),
                    .data = data
                },
                .type_index = get_type_index_of(type_to_export_type(variable.type())),
                .name = name,
                .is_initialized = variable.is_initialized()
            });
        }
    }

    static void snapshot_stackframes(ProgramSnapshot& snapshot, const InterpreterState& state, std::function<TypeIndex(const Type&)> get_type_index_of)
    {
        auto call_stack_copy = state.call_stack.copy_call_stack();
        while (not call_stack_copy.empty())
        {
            auto& stackframe = call_stack_copy.top();
            snapshot.stackframes.push_back({
                .variables = {},
                .parameters = {}
            });

            for (const auto& scope : stackframe.scope_stack.ref_scopes())
            {
                for (const auto& [name, variable] : scope)
                {
                    std::vector<Byte> data;

                    if (variable.is_initialized())
                    {
                        data.resize(variable.type().size_bytes());
                        state.memory.read_into(variable.address(), data.data(), data.size()); 
                    }

                    snapshot.stackframes.back().variables.push_back({
                        .allocated_place = {
                            .address = variable.address(),
                            .size_bytes = variable.type().size_bytes(),
                            .data = data
                        },
                        .type_index = get_type_index_of(type_to_export_type(variable.type())),
                        .name = name,
                        .is_initialized = variable.is_initialized()
                    });
                }
            }

            call_stack_copy.pop();
        }
    }

    ProgramSnapshot make_snapshot(const InterpreterState& state)
    {
        ProgramSnapshot snapshot;
        
        std::map<scc::export_format::Type, scc::export_format::TypeIndex> type_to_type_index;
        auto get_type_index_of([&type_to_type_index, &snapshot](const scc::export_format::Type& type) -> scc::export_format::TypeIndex {
            auto it = type_to_type_index.find(type);
            if (it != type_to_type_index.end())
                return it->second;

            auto type_index = type_to_type_index.size();
            type_to_type_index.insert({type, type_index});
            snapshot.types.push_back(type);

            return type_index;
        });

        snapshot_global_variables(snapshot, state, get_type_index_of);
        snapshot_stackframes(snapshot, state, get_type_index_of);

        return snapshot;
    }
}