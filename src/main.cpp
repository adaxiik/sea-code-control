
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


    scc::Parser parser;
    auto parse_result = parser.parse(code.value());
    if (parse_result.has_error())
    {
        std::cerr << "Failed to parse file: " << argv[1] << std::endl;
        return 1;
    }


    {
        std::stringstream puml;
        scc::debug::ast_as_puml(puml, parse_result);
        std::ofstream puml_file("ast.puml");
        puml_file << puml.str();
    }



   

    // {
    //     std::stringstream vars_json;
    //     scc::debug::vars_as_json(vars_json, vm);
    //     std::ofstream vars_file("vars.json");
    //     vars_file << vars_json.str();
    // }



    return 0;
}

#endif