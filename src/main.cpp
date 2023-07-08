
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
#include "instructions/instruction.hpp"
#include "instructions.hpp"
#include "vm.hpp"
#include "compiler.hpp"
#include "visitor.hpp"

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


    {
        std::stringstream puml;
        scc::debug::ast_as_puml(puml, parser);
        std::ofstream puml_file("ast.puml");
        puml_file << puml.str();
    }

    scc::Compiler compiler(parser);
    auto instructions = compiler.compile();
    if (!instructions)
    {
        std::cerr << "Failed to compile file: " << argv[1] << std::endl;
        return 1;
    }
    std::stringstream ss;

    scc::OsTextVisitor visitor(ss);
    for (auto instruction : instructions.value())
    {
        instruction->accept(visitor);
        ss << std::endl;
    }
    std::cout << ss.str();

    // std::vector<scc::Instruction*> instructions; 
    // // // int a = 34;
    // // // int b = 35;
    // // // int a = a + b;
    // // // pop to cout
    
    // instructions.push_back(new scc::instructions::NewVariable("a", scc::type::Type(scc::type::int_type{})));
    // instructions.push_back(new scc::instructions::NewVariable("b", scc::type::Type(scc::type::int_type{})));
    // instructions.push_back(new scc::instructions::PushNumber(34));
    // instructions.push_back(new scc::instructions::GetPtrToVar("a"));
    // instructions.push_back(new scc::instructions::Store(4));
    // instructions.push_back(new scc::instructions::PushNumber(35));
    // instructions.push_back(new scc::instructions::GetPtrToVar("b"));
    // instructions.push_back(new scc::instructions::Store(4));
    // instructions.push_back(new scc::instructions::GetPtrToVar("a"));
    // instructions.push_back(new scc::instructions::Load(4));
    // instructions.push_back(new scc::instructions::GetPtrToVar("b"));
    // instructions.push_back(new scc::instructions::Load(4));
    // instructions.push_back(new scc::instructions::Add(scc::type::Type(scc::type::int_type{})));
    // instructions.push_back(new scc::instructions::GetPtrToVar("a"));
    // instructions.push_back(new scc::instructions::Store(4));

    // instructions.push_back(new scc::instructions::GetPtrToVar("a"));
    // instructions.push_back(new scc::instructions::Load(4)); 
    // instructions.push_back(new scc::instructions::DPrint());


    scc::vm::VM vm{std::move(instructions.value())};
    auto error = vm.run();
    if (error != scc::vm::Error::None)
    {
        std::cerr << "VM error: " << static_cast<int>(error) << std::endl;
        return 1;
    }

    // std::cout << "VM stack size: " << vm.get_vm_stack_size() << std::endl;


    {
        std::stringstream vars_json;
        scc::debug::vars_as_json(vars_json, vm);
        std::ofstream vars_file("vars.json");
        vars_file << vars_json.str();
    }


    auto to_delete = vm.clear_instructions(); 
    for (auto i : to_delete) // TODOOO, stop allocating every single instruction
        delete i;


    return 0;
}

#endif