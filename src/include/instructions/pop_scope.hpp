#pragma once
#include <string>
#include "type.hpp"
#include "vm.hpp"
#include "./instruction.hpp"

namespace scc
{
    namespace instructions
    {
        struct PopScope : public Instruction
        {
            virtual vm::Error execute(vm::VM& vm) override;

            virtual void accept(Visitor &v) const override{ v.visit(*this); }
        };
        
        #ifdef SCC_POP_SCOPE_IMPLEMENTATION
        vm::Error PopScope::execute(vm::VM& vm)
        {
            vm.ref_scope_stack().pop();
            return vm::Error::None;
        }
        #endif
    } 
} 