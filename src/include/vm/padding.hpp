#pragma once
#include <cstdint>
#include <cstddef>

namespace scc
{
    namespace vm
    {
        struct Padding
        {
            using PaddingType = uint32_t;
            constexpr static size_t Size = sizeof(PaddingType);
            PaddingType data;
        };
        static_assert(sizeof(Padding) == Padding::Size, "Padding is not the correct size");
    }
}