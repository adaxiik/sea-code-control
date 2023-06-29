#pragma once
#include <string>
#include "type.hpp"
namespace scc
{
    struct NewVar
    {
        std::string name;
        type::Type type;
    };

    using Instruction = std::variant<NewVar>;
}
