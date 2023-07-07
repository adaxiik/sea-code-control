#pragma once
#include "vm.hpp"
#include "visitor.hpp"
namespace scc
{
    struct Instruction
    {
        virtual vm::Error execute(vm::VM& vm) = 0;
        virtual ~Instruction() = default;

        virtual void accept(Visitor& ) const = 0;
    };

}
