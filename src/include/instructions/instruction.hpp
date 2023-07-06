#pragma once
#include "vm.hpp"
namespace scc
{
    struct Instruction
    {
        virtual vm::Error execute(vm::VM& vm) = 0;
        virtual ~Instruction() = default;
    };

}
