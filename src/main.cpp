
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


#include <scc/debug.hpp>
#include <scc/io.hpp>
#include <scc/parser.hpp>
#include <scc/interpreter.hpp>
#include <scc/repl.hpp>

int main(int argc, char const *argv[])
{

    if (argc < 2)
    {
        // REPL

        scc::REPL repl;
        repl.run();
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

    auto running_intepreter_result = interpreter.interpret(parse_result);
    if (running_intepreter_result.is_error())
    {
        auto binder_error = running_intepreter_result.binder_error;
        std::cerr << "Binder error" << std::endl;

        return 1;
    }

    auto running_interpreter = running_intepreter_result.value();
    auto interpret_result = running_interpreter.continue_execution();

    if (interpret_result.is_error())
    {
        std::cerr << "Interpret error" << std::endl;
        scc::debug::interpreter_error_as_text(std::cerr, interpret_result.get_error());
        std::cerr << std::endl;
        return 1;
    }

    return 0;
}

#endif