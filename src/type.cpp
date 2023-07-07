#include "type.hpp"

namespace scc
{
    namespace type
    {
        size_t ptr_type::size_bytes() const
        {
            if (is_static_array)
                return static_cast<size_t>(size * pointing_to->size_bytes());
            else
                return 8UL;
        }

        std::ostream& operator<<(std::ostream& os, const ptr_type& ptr)
        {
            os << *ptr.pointing_to << "*";
            return os;
        }
    }
} 
