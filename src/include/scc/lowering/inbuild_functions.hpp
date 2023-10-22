#pragma once
#include "interpreter_state.hpp"
#include <map>
#include <string>
#include <functional>

namespace scc::lowering::inbuild
{
    using FunctionName = std::string;
    using InbuildFunction = std::function<InterpreterResult(InterpreterState&)>;
    
    struct Function
    {
        InbuildFunction inbuild_function;
        Type return_type;
        std::vector<Type> argument_types;
    };

    InterpreterResult puti(InterpreterState &state);
    InterpreterResult putc(InterpreterState &state);
    
    const std::map<FunctionName, Function> inbuild_functions = {
        {"_scc_puti", {puti, Type(Type::Kind::Void), {Type(Type::Kind::I32)}}},
        {"_scc_putc", {putc, Type(Type::Kind::Void), {Type(Type::Kind::Char)}}},
    };
}