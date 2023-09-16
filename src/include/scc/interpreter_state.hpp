#pragma once
#include <stack>
#include "memory.h"
#include "interpreter_scope.hpp"
namespace scc
{
    struct InterpreterState
    {
        using Label = size_t;
        using InstructionPointer = size_t;
        
        Memory memory;
        InterpreterScopeStack scope_stack;
        std::stack<InterpreterResult> result_stack;
        InstructionPointer instruction_pointer;
        std::map<Label, InstructionPointer> labels;
        // std::map<std::string, std::unique_ptr<binding::BoundFunctionStatement>> functions;
    };
}