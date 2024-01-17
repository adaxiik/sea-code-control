#pragma once
#include "interpreter_state.hpp"
#include <map>
#include <string>
#include <functional>
#include <vector>

namespace scc::lowering::inbuild
{
    using FunctionName = std::string;
    using InbuildFunction = std::function<InterpreterResult(InterpreterState &)>;

    struct Function
    {
        InbuildFunction inbuild_function;
        Type return_type;
        std::vector<Type> argument_types;
    };

    InterpreterResult puti(InterpreterState &state);
    InterpreterResult putc(InterpreterState &state);
    InterpreterResult assert(InterpreterState &state);
    InterpreterResult malloc(InterpreterState &state);
    InterpreterResult free(InterpreterState &state);
    InterpreterResult write(InterpreterState &state);

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
        {"_scc_assert", {assert, Type(Type::PrimitiveType::Void), {Type(Type::PrimitiveType::Bool)}}},
        {"_scc_malloc", {malloc, Type(Type::PrimitiveType::Void, std::vector<Type::Modifier>{Type::Pointer{}}), {Type(Type::PrimitiveType::U64)}}},
        {"_scc_free", {free, Type(Type::PrimitiveType::Void), {Type(Type::PrimitiveType::Void, std::vector<Type::Modifier>{Type::Pointer{}})}}},
        {"_scc_write", {
            write,
            Type(Type::PrimitiveType::I64),
            {
                Type(Type::PrimitiveType::I32),
                Type(
                    Type::PrimitiveType::Void,
                    std::vector<Type::Modifier>{Type::Pointer{}}
                ),
                Type(Type::PrimitiveType::U64)
            }
        }}
    };

}