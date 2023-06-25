#pragma once

#include <optional>
#include <string>
namespace scc
{
    namespace io
    {
        std::optional<std::string> slurp_file(const std::string &filename);
    } 
} 
