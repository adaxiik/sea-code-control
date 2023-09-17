#pragma once
#include <stack>
#include <map>
#include <string>
#include "memory.h"
#include "interpreter_scope.hpp"
#include "call_stack.hpp"
#include "global_scope.hpp"

namespace scc
{
    struct InterpreterState
    {
        using Label = size_t;
        using InstructionPointer = size_t;
        
        Memory memory;
        std::stack<InterpreterResult> result_stack;
        InstructionPointer instruction_pointer;
        std::map<Label, InstructionPointer> labels;
        std::map<std::string, InstructionPointer> functions;
        CallStack call_stack;
        GlobalScope global_scope;
        InterpreterState(size_t memory_offset, size_t stack_size, size_t global_scope_size)
        : memory(Memory(memory_offset))
        , call_stack(CallStack(memory.allocate(stack_size) + stack_size))
        , global_scope(GlobalScope(memory.allocate(global_scope_size) + global_scope_size)) {} // global scope shifts the address to "left" as well as the stack
    };
}