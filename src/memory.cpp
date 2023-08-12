#include "memory.hpp"
#include <cstring>
namespace scc
{
    Memory::Memory()
        : m_memory(std::make_unique<char[]>(INITIAL_SIZE))
        , m_allocated(INITIAL_SIZE)
        , m_used(0)
        , m_next_id(0)
    {
    }

    Memory::MemoryChunkId Memory::allocate(uint32_t size)
    {
        for (auto& [id, chunk] : m_chunks)
        {
            if (!chunk.is_free)
                continue;
            
            if (chunk.size_bytes >= size)
            {
                // I create new subchunk from start and leave the rest as free
                size_t left_size = chunk.size_bytes - size;
                if (left_size != 0)
                {
                    MemoryChunkId new_id = m_next_id++;
                    m_chunks[new_id] = { chunk.start_address + size, left_size, true }; 
                }
                chunk.size_bytes = size;
                chunk.is_free = false;
                return id;
            }
        }


        MemoryChunkId new_id = m_next_id++;
        m_chunks[new_id] = { m_used, size, false };
        m_used += size;

        if (m_used > m_allocated)
        {
            size_t new_size = m_allocated * 2;
            std::unique_ptr<char[]> new_memory = std::make_unique<char[]>(new_size);
            std::memcpy(new_memory.get(), m_memory.get(), m_allocated);
            m_memory = std::move(new_memory);
            m_allocated = new_size;
        }

        return new_id;
    }

    void Memory::free(MemoryChunkId current_id)
    {
        auto it = m_chunks.find(current_id);
        if (it == m_chunks.end())
            return;
        
        MemoryChunk& current_chunk = it->second;
        current_chunk.is_free = true;

        // we will concat with next or previous chunk if it is free
        
        std::optional<MemoryChunkId> delete_left;
        std::optional<MemoryChunkId> delete_right;
        for (auto& [id, chunk] : m_chunks)
        {
            if (!chunk.is_free)
                continue;

            address_t current_chunk_begin = get_chunk_begin(current_id);
            address_t current_chunk_end = get_chunk_end(current_id);
            address_t chunk_begin = get_chunk_begin(id);
            address_t chunk_end = get_chunk_end(id);

            // chunk | current_chunk | chunk

            if (current_chunk_begin - 1 == chunk_end)
            {
                // chunk | current_chunk
                current_chunk.size_bytes += chunk.size_bytes;
                current_chunk.start_address = chunk.start_address;
                delete_left = id;
            }
            else if (current_chunk_end + 1 == chunk_begin)
            {
                // current_chunk | chunk
                current_chunk.size_bytes += chunk.size_bytes;
                delete_right = id;
            }
        }

        if (delete_left)
            m_chunks.erase(delete_left.value());
        
        if (delete_right)
            m_chunks.erase(delete_right.value());

    }

    Memory::address_t Memory::get_chunk_begin(MemoryChunkId id) const
    {
        return m_chunks.at(id).start_address;
    }
    Memory::address_t Memory::get_chunk_end(MemoryChunkId id) const
    {
        return m_chunks.at(id).start_address + m_chunks.at(id).size_bytes - 1;
    }
    Memory::address_t Memory::get_chunk_size(MemoryChunkId id) const
    {
        return m_chunks.at(id).size_bytes;
    }
    bool Memory::is_chunk_free(MemoryChunkId id) const
    {
        return m_chunks.at(id).is_free;
    }
}
