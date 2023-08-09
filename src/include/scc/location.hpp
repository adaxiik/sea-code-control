#pragma once
#include <ostream>
#include <cstdint>
namespace scc
{
    struct Location
    {
        uint32_t row;
        uint32_t column;

        std::ostream &operator<<(std::ostream &os) const
        {
            os << "at " << row << ":" << column;
            return os;
        }
    };
}