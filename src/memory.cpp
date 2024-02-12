#include "memory.hpp"
#include <cstring>
namespace scc
{
    Memory::address_t Memory::allocate(size_t size, bool is_protected)
    {
        address_t address = m_next_address;
        m_next_address += size;

        m_memory.insert({address, MemoryChunk(size, is_protected)});

        return address;
    }

    bool Memory::write_buffer_unsafe(address_t address, const void* buffer, size_t size)
    {
        std::optional<address_t> start_address_opt = find_start_of_chunk(address);
        if (not start_address_opt.has_value())
            return false;

        address_t start_address = start_address_opt.value();
        address_t real_memory_index = address - start_address;

        if (real_memory_index + size > m_memory.at(start_address).size)
            return false;

        std::memcpy(&m_memory.at(start_address).data[real_memory_index], buffer, size);

        return true;
    }

    bool Memory::free(address_t address)
    {
        auto it = m_memory.find(address);
        if (it == m_memory.end())
            return false;
        m_memory.erase(it);
        return true;
    }

    std::optional<Memory::address_t> Memory::find_start_of_chunk(address_t address) const
    {
        for(auto& [start, chunk] : m_memory)
        {
            if (start <= address && address < start + chunk.size)
                return start;
        }
        return std::nullopt;
    }

    std::optional<Memory::address_t> Memory::get_chunk_end(Memory::address_t address) const
    {
        auto start = find_start_of_chunk(address);
        if (not start)
            return std::nullopt;

        auto it = m_memory.find(start.value());
        if (it == m_memory.end())
            return std::nullopt;

        return it->second.size + it->first - 1;
    }

    std::optional<size_t> Memory::get_chunk_size (Memory::address_t address) const
    {
        auto start = find_start_of_chunk(address);
        if (not start)
            return std::nullopt;

        auto it = m_memory.find(start.value());
        if (it == m_memory.end())
            return std::nullopt;

        return it->second.size;
    }

    size_t Memory::allocated_chunk_count() const { return m_memory.size(); }
    size_t Memory::allocated_memory_size_bytes() const
    {
        size_t size = 0;
        for(auto& [start, chunk] : m_memory)
            size += chunk.size;
        return size;
    }

    bool Memory::read_buffer(address_t address, void* buffer, size_t size) const
    {
        std::optional<address_t> start_address_opt = find_start_of_chunk(address);
        if (not start_address_opt.has_value())
            return false;

        address_t start_address = start_address_opt.value();
        address_t real_memory_index = address - start_address;

        if (real_memory_index + size > m_memory.at(start_address).size)
            return false;

        std::memcpy(buffer, &m_memory.at(start_address).data[real_memory_index], size);

        return true;
    }

    std::optional<Type::Value> Memory::read_value(address_t address, Type type) const
    {
        #define CASE_GET_VALUE(KIND) case Type::PrimitiveType::KIND: \
            { \
                auto value = read<Type::Primitive::KIND>(address); \
                if (not value.has_value()) \
                    return std::nullopt; \
                return Type::Value(value.value()); \
            } \
            break;

        if (type.is_pointer())
        {
            auto value = read<Type::Primitive::U64>(address);
            if (not value.has_value())
                return std::nullopt;
            return Type::Value(value.value());
        }

        if (type.is_struct())
        {
            Type::StructValue struct_value;
            size_t offset = 0;
            for (const auto& field : std::get<Type::StructType>(type.base_type).fields)
            {
                auto value = read_value(address + offset, field.second);
                if (not value.has_value())
                    return std::nullopt;

                struct_value.fields.insert({field.first, value.value().value});
                offset += field.second.size_bytes();
            }
            return Type::Value(type, struct_value);
        }

        static_assert(std::variant_size_v<Type::BaseType> == 2);
        if (std::holds_alternative<Type::PrimitiveType>(type.base_type))
        {
            static_assert(static_cast<int>(Type::PrimitiveType::COUNT) == 13);
            switch (std::get<Type::PrimitiveType>(type.base_type))
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
                default:
                    return std::nullopt;
            }
        }
        else
        {
            // TODOOO: implement struct variables
            std::cerr << "Struct variables not implemented yet " << __FILE__ << ":" << __LINE__ << std::endl;
            std::abort();
        }
    }

    bool Memory::write_value(address_t address, Type::Value value, Type type)
    {
        #define CASE_SET_VALUE(KIND) case Type::PrimitiveType::KIND: \
            return write<Type::Primitive::KIND>(address, std::get<Type::Primitive::KIND>(std::get<Type::PrimitiveValue>(value.value)));

         if (type.is_pointer())
            return write<Type::Primitive::U64>(address, std::get<Type::Primitive::U64>(std::get<Type::PrimitiveValue>(value.value)));

        if (type.is_struct())
        {
            // TODOOO: implement struct variables
            std::cerr << "Struct variables not implemented yet " << __FILE__ << ":" << __LINE__ << std::endl;
            std::abort();
        }

        switch (std::get<Type::PrimitiveType>(type.base_type))
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

            case Type::PrimitiveType::COUNT:
            default:
                return false;
        }
    }

    bool Memory::memset(address_t address, char value, size_t size)
    {
        if (size == 0)
            return true;

        std::optional<address_t> start_address_opt = find_start_of_chunk(address);
        if (not start_address_opt.has_value())
            return false;

        address_t start_address = start_address_opt.value();
        address_t real_memory_index = address - start_address;

        if (real_memory_index + size > m_memory.at(start_address).size)
            return false;

        std::memset(&m_memory.at(start_address).data[real_memory_index], value, size);

        return true;
    }

    std::optional<size_t> Memory::find_first_byte_value_offset_in_chunk(address_t address, char value) const
    {
        std::optional<address_t> start_address_opt = find_start_of_chunk(address);
        if (not start_address_opt.has_value())
            return std::nullopt;

        address_t start_address = start_address_opt.value();
        address_t real_memory_index = address - start_address;

        if (real_memory_index >= m_memory.at(start_address).size)
            return std::nullopt;

        for (size_t i = real_memory_index; i < m_memory.at(start_address).size; ++i)
            if (m_memory.at(start_address).data[i] == value)
                return i - real_memory_index;

        return std::nullopt;

    }

}
