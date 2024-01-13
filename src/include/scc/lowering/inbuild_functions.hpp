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

    // math.h
    // https://en.cppreference.com/w/c/numeric/math
    InterpreterResult sinf(InterpreterState &state);
    InterpreterResult cosf(InterpreterState &state);
    InterpreterResult sin(InterpreterState &state);
    InterpreterResult cos(InterpreterState &state);

    const std::map<FunctionName, Function> inbuild_functions = {
        {"_scc_puti", {puti, Type(Type::PrimitiveType::Void), {Type(Type::PrimitiveType::I32)}}},
        {"_scc_putc", {putc, Type(Type::PrimitiveType::Void), {Type(Type::PrimitiveType::Char)}}},
        {"_scc_sinf", {sinf, Type(Type::PrimitiveType::F32), {Type(Type::PrimitiveType::F32)}}},
        {"_scc_cosf", {cosf, Type(Type::PrimitiveType::F32), {Type(Type::PrimitiveType::F32)}}},
        {"_scc_sin", {sin, Type(Type::PrimitiveType::F64), {Type(Type::PrimitiveType::F64)}}},
        {"_scc_cos", {cos, Type(Type::PrimitiveType::F64), {Type(Type::PrimitiveType::F64)}}},
    };
}