#pragma once

#include <memory>
#include <map>
#include <optional>
#include <cstdlib>

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
        Memory(const Memory&) = delete;
        Memory(Memory&&) = default;
        Memory& operator=(const Memory&) = delete;
        Memory& operator=(Memory&&) = default;

        struct MemoryChunk
        {
            std::unique_ptr<char[]> data;
            size_t size;
            bool is_protected;

            MemoryChunk(size_t size, bool is_protected = false)
            : data(std::make_unique<char[]>(size))
            , size(size)
            , is_protected(is_protected)
            {
                for (size_t i = 0; i < size; i++)
                    data[i] = std::rand() % 256; // oh no, C style rand :O

            }
            MemoryChunk(const MemoryChunk&) = delete;
            MemoryChunk(MemoryChunk&&) = default;
            MemoryChunk& operator=(const MemoryChunk&) = delete;
            MemoryChunk& operator=(MemoryChunk&&) = default;
        };

        address_t allocate(size_t size, bool is_protected = false);

        /**
         * @brief free memory
         *
         * @param address
         * @return true if memory was freed successfully
         * @return false if address was not found
         */
        bool free(address_t address);

        std::optional<address_t> find_start_of_chunk(address_t address) const;

        std::optional<Type::Value> read_value(address_t address, Type type) const;

        /**
         * @brief write value to memory
         *
         * @param address
         * @param value
         * @param type
         * @return true if value was written successfully
         * @return false if address was not found
         */
        bool write_value(address_t address, Type::Value value, Type type);

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


            // std::cout << "READ: " << address << " " << *reinterpret_cast<T*>(&m_memory.at(start_address).data[real_memory_index]) << std::endl;
            return *reinterpret_cast<T*>(&m_memory.at(start_address).data[real_memory_index]);
        }

        /**
         * @brief read value from memory
         *
         * @param address
         * @param buffer
         * @param size
         * @return true if value was read successfully
         * @return false if address was not found or size is too big
         */
        bool read_buffer(address_t address, void* buffer, size_t size) const;

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
            // std::cout << "WRITE: " << address << " " << value << std::endl;

            static_assert(sizeof(T) <= 8, "T is too big");
            std::optional<address_t> start_address_opt = find_start_of_chunk(address);
            if (not start_address_opt.has_value())
                return false;

            address_t start_address = start_address_opt.value();
            address_t real_memory_index = address - start_address;

            if (real_memory_index + sizeof(T) > m_memory.at(start_address).size)
                return false;

            auto& chunk = m_memory.at(start_address);
            if (chunk.is_protected)
                return false;

            *reinterpret_cast<T*>(&chunk.data[real_memory_index]) = value;

            return true;
        }

        /**
         * @brief write value to memory
         *
         * @warning it ignores chunk protection
         *
         * @param address
         * @param buffer
         * @param size
         * @return true if value was written successfully
         * @return false if address was not found or size is too big
         */
        bool write_buffer_unsafe(address_t address, const void* buffer, size_t size);

        /**
         * @brief set memory to value
         *
         * @param address
         * @param value
         * @param size
         * @return true if memory was set successfully
         * @return false if address was not found or size is too big
         */
        bool memset(address_t address, char value, size_t size);

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