#include "memory.hpp"
#include <cstring>
namespace scc
{
    Memory::address_t Memory::allocate(size_t size)
    {
        address_t address = m_next_address;
        m_next_address += size;

        m_memory.insert({address, MemoryChunk(size)});

        return address;
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
        if (!start)
            return std::nullopt;
        
        auto it = m_memory.find(start.value());
        if (it == m_memory.end())
            return std::nullopt;
        
        return it->second.size + it->first - 1;
    }

    std::optional<size_t> Memory::get_chunk_size (Memory::address_t address) const
    {
        auto start = find_start_of_chunk(address);
        if (!start)
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
}
