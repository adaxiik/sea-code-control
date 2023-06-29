
#define UNUSED(x) (void)(x)

#ifdef BUILDING_WASM
#include "api.hpp"
#else

#include <iostream>
#include <fstream>
#include <functional>
#include <optional>
#include <sstream>
#include <string>


#include "debug.hpp"
#include "io.hpp"
#include "parser.hpp"
#include "vm.hpp"

int main(int argc, char const *argv[])
{

    if (argc < 2)
    {
        std::cerr << "Usage: " << argv[0] << " <filename>" << std::endl;
        return 1;
    }

    auto code = scc::io::slurp_file(argv[1]);
    if (!code)
    {
        std::cerr << "Failed to open file: " << argv[1] << std::endl;
        return 1;
    }


    scc::Parser parser(code.value());
    if (parser.has_error())
    {
        std::cerr << "Failed to parse file: " << argv[1] << std::endl;
        return 1;
    }


    // std::cout << scc::debug::ast_as_puml(parser).str() << std::endl;

    std::vector<scc::Instruction> instructions;
    instructions.push_back(scc::NewVar{"a", scc::type::Type{scc::type::I32{}, false}});

    scc::vm::VM vm{std::move(instructions)};
    auto error = vm.run();
    if (error != scc::vm::Error::None)
    {
        std::cerr << "VM error: " << static_cast<int>(error) << std::endl;
        return 1;
    }

    return 0;
}

#endif