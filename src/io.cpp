#include "io.hpp"

#include <fstream>
#include <sstream>

std::optional<std::string> scc::io::slurp_file(const std::string &filename)
{
    std::ifstream file(filename);
    if (!file.is_open())
        return std::nullopt;

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}