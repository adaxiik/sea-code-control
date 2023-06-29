#pragma once
#include <string>
#include "type.hpp"
namespace scc
{
    // TODOO: namespace?
    struct NewVar
    {
        std::string name;
        type::Type type;
    };

    struct PushScope{};
    struct PopScope{};

    struct GetPtrToVar
    {
        std::string name;
    };

    struct Load
    {
        uint8_t size_bytes;
    };

    struct Store
    {
        uint8_t size_bytes;
    };

    struct PushNumber{ uint64_t value; };
    struct Add{
        type::Type result_type;
    };


    using Instruction = std::variant<NewVar
                                   , PushScope
                                   , PopScope
                                   , GetPtrToVar
                                   , Load
                                   , Store
                                   , PushNumber
                                   , Add
                                      >;
}
