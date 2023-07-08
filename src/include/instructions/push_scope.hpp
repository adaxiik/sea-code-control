#pragma once
#include <string>
#include "type.hpp"
#include "vm.hpp"
#include "./instruction.hpp"

namespace scc
{
    namespace instructions
    {
        struct PushScope : public Instruction
        {
            virtual vm::Error execute(vm::VM& vm) override;

            virtual void accept(InstructionVisitor &v) const override{ v.visit(*this); }

        };
        
        #ifdef SCC_PUSH_SCOPE_IMPLEMENTATION
        vm::Error PushScope::execute(vm::VM& vm)
        {
            vm.ref_scope_stack().push();
            return vm::Error::None;
        }
        #endif
    } 
} 