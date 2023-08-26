#pragma once

#include <memory>
#include <map>
#include <optional>
#include "type.hpp"
namespace scc
{
    // low level memory manager
    class Memory
    {
    public:
        static_assert(sizeof(char) == 1, "char is not 1 byte");
        constexpr static auto BYTE = 1;
        constexpr static auto KILOBYTE = 1024 * BYTE;
        constexpr static auto MEGABYTE = 1024 * KILOBYTE;
        constexpr static auto GIGABYTE = 1024 * MEGABYTE;

        using address_t = Type::Primitive::U64;
        constexpr static address_t DEFAULT_ADDRESS_OFFSET = 8 * MEGABYTE;

        Memory(address_t offset = DEFAULT_ADDRESS_OFFSET) : m_next_address(offset) {}
        ~Memory() = default;

        struct MemoryChunk
        {
            std::unique_ptr<char[]> data;
            size_t size;

            MemoryChunk(size_t size) : data(std::make_unique<char[]>(size)), size(size) {}
            MemoryChunk(const MemoryChunk&) = delete;
            MemoryChunk(MemoryChunk&&) = default;
            MemoryChunk& operator=(const MemoryChunk&) = delete;
            MemoryChunk& operator=(MemoryChunk&&) = default;
        };

        address_t allocate(size_t size);

        /**
         * @brief free memory
         * 
         * @param address 
         * @return true if memory was freed successfully
         * @return false if address was not found
         */
        bool free(address_t address);
 
        std::optional<address_t> find_start_of_chunk(address_t address) const;


        template<typename T>
        std::optional<T> read(address_t address) const
        {
            static_assert(sizeof(T) <= 8, "T is too big");
            std::optional<address_t> start_address_opt = find_start_of_chunk(address);
            if (!start_address_opt.has_value())
                return std::nullopt;
            
            address_t start_address = start_address_opt.value();
            address_t real_memory_index = address - start_address;

            if (real_memory_index + sizeof(T) > m_memory.at(start_address).size)
                return std::nullopt;
            
            return *reinterpret_cast<T*>(&m_memory.at(start_address).data[real_memory_index]);
        }

        /**
         * @brief write value to memory
         * 
         * @tparam T 
         * @param address
         * @param value 
         * @return true if value was written successfully
         * @return false if address was not found
         */
        template<typename T>
        bool write(address_t address, T value)
        {
            static_assert(sizeof(T) <= 8, "T is too big");
            std::optional<address_t> start_address_opt = find_start_of_chunk(address);
            if (!start_address_opt.has_value())
                return false;
            
            address_t start_address = start_address_opt.value();
            address_t real_memory_index = address - start_address;
            
            if (real_memory_index + sizeof(T) > m_memory.at(start_address).size)
                return false;

            *reinterpret_cast<T*>(&m_memory.at(start_address).data[real_memory_index]) = value;
            
            return true;
        }

        const std::map<address_t, MemoryChunk>& get_chunks() const { return m_memory; }

        std::optional<address_t> get_chunk_end (address_t address) const;
        std::optional<size_t> get_chunk_size (address_t address) const;

        size_t allocated_chunk_count() const;
        size_t allocated_memory_size_bytes() const;
    private:
        address_t m_next_address;
        std::map<address_t,  MemoryChunk> m_memory;
    };
}