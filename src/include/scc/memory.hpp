#pragma once

#include <memory>
#include <map>
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

        using MemoryChunkId = uint32_t;
        using address_t = uint64_t;
        constexpr static size_t INITIAL_SIZE = 1 * BYTE;

        Memory();
        ~Memory() = default;
        
        MemoryChunkId allocate(uint32_t size);
        void free(MemoryChunkId id);

        
        template<typename T>
        void write(address_t address, T value)
        {
            *reinterpret_cast<T*>(m_memory.get() + address) = value;
        }

        template<typename T>
        T read(address_t address)
        {
            return *reinterpret_cast<T*>(m_memory.get() + address);
        }

        address_t get_chunk_begin(MemoryChunkId id) const;
        address_t get_chunk_end(MemoryChunkId id) const;
        address_t get_chunk_size(MemoryChunkId id) const;
        bool is_chunk_free(MemoryChunkId id) const;


        struct MemoryChunk
        {
            address_t start_address;
            uint64_t size_bytes;
            bool is_free;
        };
        const std::map<MemoryChunkId, MemoryChunk>& get_chunks() const { return m_chunks; }
    
    private:
       
        std::unique_ptr<char[]> m_memory;
        size_t m_allocated;
        size_t m_used;

        MemoryChunkId m_next_id;

        std::map<MemoryChunkId, MemoryChunk> m_chunks;
    };
}