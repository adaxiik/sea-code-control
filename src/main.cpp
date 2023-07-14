
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
#include "interpreter.hpp"


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
    
    scc::Interpreter interpreter;
    auto parse_result = interpreter.parse(code.value());
    if (parse_result.has_error())
    {
        std::cerr << "Parse error" << std::endl;
        return 1;
    }

    {
        std::stringstream puml;
        scc::debug::ast_as_puml(puml, parse_result);
        std::ofstream puml_file("ast.puml");
        puml_file << puml.str();
    }

    auto interpret_result = interpreter.interpret(parse_result);

    if (interpret_result == scc::InterpreterResult::ParseError)
    {
        std::cerr << "Parse error" << std::endl;
        return 1;
    }

    return 0;
}

#endif