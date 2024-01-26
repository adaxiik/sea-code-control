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
            [&j](const ArrayType& array_type) {
                j["element_type_index"] = array_type.element_type_index;
                j["element_count"] = array_type.element_count;
            },
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
        j = nlohmann::json{
            {"variables", stackframe.variables},
            {"parameters", stackframe.parameters},
            {"function_name", stackframe.function_name}
        };
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

    static scc::export_format::PrimitiveType primitive_t_to_export_primitive_t(scc::Type::PrimitiveType primitive_type)
    {
        static_assert(static_cast<int>(scc::Type::PrimitiveType::COUNT) == 13);
        switch (primitive_type)
        {
        case scc::Type::PrimitiveType::Char: return scc::export_format::PrimitiveType{scc::export_format::CharType{}};
        case scc::Type::PrimitiveType::I8:   return scc::export_format::PrimitiveType{scc::export_format::I8Type{}};
        case scc::Type::PrimitiveType::I16:  return scc::export_format::PrimitiveType{scc::export_format::I16Type{}};
        case scc::Type::PrimitiveType::I32:  return scc::export_format::PrimitiveType{scc::export_format::I32Type{}};
        case scc::Type::PrimitiveType::I64:  return scc::export_format::PrimitiveType{scc::export_format::I64Type{}};
        case scc::Type::PrimitiveType::U8:   return scc::export_format::PrimitiveType{scc::export_format::U8Type{}};
        case scc::Type::PrimitiveType::U16:  return scc::export_format::PrimitiveType{scc::export_format::U16Type{}};
        case scc::Type::PrimitiveType::U32:  return scc::export_format::PrimitiveType{scc::export_format::U32Type{}};
        case scc::Type::PrimitiveType::U64:  return scc::export_format::PrimitiveType{scc::export_format::U64Type{}};
        case scc::Type::PrimitiveType::F32:  return scc::export_format::PrimitiveType{scc::export_format::F32Type{}};
        case scc::Type::PrimitiveType::F64:  return scc::export_format::PrimitiveType{scc::export_format::F64Type{}};
        case scc::Type::PrimitiveType::Bool: return scc::export_format::PrimitiveType{scc::export_format::BoolType{}};
        default:
            std::cerr << "type_to_export_type: unknown type kind: " << static_cast<int>(primitive_type) << std::endl;
            std::abort();
            break;
        }
    }

    static void snapshot_global_variables(ProgramSnapshot& snapshot, const InterpreterState& state, std::function<scc::export_format::TypeIndex(const scc::Type&)>  get_type_index_of)
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
                .type_index = get_type_index_of(variable.type()),
                .name = name,
                .is_initialized = variable.is_initialized()
            });
        }

        std::sort(snapshot.global_variables.begin(), snapshot.global_variables.end(), [](const auto& a, const auto& b) { return a.allocated_place.address > b.allocated_place.address; });
    }

    static void snapshot_stackframes(ProgramSnapshot& snapshot, const InterpreterState& state, std::function<scc::export_format::TypeIndex(const scc::Type&)>  get_type_index_of)
    {
        auto call_stack_copy = state.call_stack.copy_call_stack();
        while (not call_stack_copy.empty())
        {
            auto& stackframe = call_stack_copy.top();
            snapshot.stackframes.push_back({
                .function_name = stackframe.function_name,
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

                    export_format::Variable exported_variable{
                        .allocated_place = {
                            .address = variable.address(),
                            .size_bytes = variable.type().size_bytes(),
                            .data = data
                        },
                        .type_index = get_type_index_of(variable.type()),
                        .name = name,
                        .is_initialized = variable.is_initialized()
                    };

                    if (variable.is_parameter())
                        snapshot.stackframes.back().parameters.push_back(std::move(exported_variable));
                    else
                        snapshot.stackframes.back().variables.push_back(std::move(exported_variable));
                }

                std::sort(snapshot.stackframes.back().variables.begin(), snapshot.stackframes.back().variables.end(), [](const auto& a, const auto& b) { return a.allocated_place.address > b.allocated_place.address; });
                std::sort(snapshot.stackframes.back().parameters.begin(), snapshot.stackframes.back().parameters.end(), [](const auto& a, const auto& b) { return a.allocated_place.address > b.allocated_place.address; });
            }

            call_stack_copy.pop();
        }

        snapshot.stackframes.pop_back(); // remove the last stackframe.. its empty
        // we'll reverse it, to have the most recent stackframe at the end
        std::reverse(snapshot.stackframes.begin(), snapshot.stackframes.end());
    }

    ProgramSnapshot make_snapshot(const InterpreterState& state)
    {
        ProgramSnapshot snapshot;
        std::unordered_map<scc::Type, scc::export_format::TypeIndex> type_to_type_index;

        std::function<scc::export_format::TypeIndex(const scc::Type&)> get_type_index_of;
        get_type_index_of =[&type_to_type_index, &snapshot, &get_type_index_of](const scc::Type& type) -> scc::export_format::TypeIndex {

            auto it = type_to_type_index.find(type);
            if (it != type_to_type_index.end())
                return it->second;



            if (type.is_struct())
            {
                std::cerr << "type_to_export_type: struct type is not supported yet" << std::endl;
                std::abort();
            }

            if (type.is_pointer())
            {
                // well.. rekurzivní typy by přecejen byly fajn xd
                auto type_copy = type;

                scc::Type::Modifier last_modifier = type.modifiers.back();
                type_copy.modifiers.pop_back();
                export_format::TypeIndex pointing_to_type_index = get_type_index_of(type_copy);

                std::visit(overloaded{
                    [&](const scc::Type::Pointer) { snapshot.types.push_back(PointerType{pointing_to_type_index}); },
                    [&](const scc::Type::Array array) { snapshot.types.push_back(ArrayType{pointing_to_type_index, type.size_bytes(), array.size}); },
                }, last_modifier);

                auto type_index = type_to_type_index.size();
                type_to_type_index.insert({type, type_index});
                return type_index;
            }

            snapshot.types.push_back(primitive_t_to_export_primitive_t(type.primitive_type().value_or(scc::Type::PrimitiveType::COUNT)));

            auto type_index = type_to_type_index.size();
            type_to_type_index.insert({type, type_index});
            return type_index;
        };

        snapshot_global_variables(snapshot, state, get_type_index_of);
        snapshot_stackframes(snapshot, state, get_type_index_of);

        return snapshot;
    }
}