#pragma once
#include "memory.h"
#include "interpreter_scope.hpp"
namespace scc
{
    struct InterpreterState
    {
        Memory memory;
        InterpreterScopeStack scope_stack;
        // std::map<std::string, std::unique_ptr<binding::BoundFunctionStatement>> functions;
    };
}