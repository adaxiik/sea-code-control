#pragma once
#include <stack>
#include "memory.h"
#include "interpreter_scope.hpp"
namespace scc
{
    struct InterpreterState
    {
        Memory memory;
        InterpreterScopeStack scope_stack;
        std::stack<InterpreterResult> result_stack;
        // std::map<std::string, std::unique_ptr<binding::BoundFunctionStatement>> functions;
    };
}