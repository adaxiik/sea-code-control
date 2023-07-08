#pragma once
#include <string>
#include <cstring>
#include <cassert>
#include "type.hpp"
#include "vm.hpp"
#include "./instruction.hpp"

namespace scc
{
    namespace instructions
    {
        struct Drop : public Instruction
        {
          
            virtual vm::Error execute(vm::VM &vm) override;
            Drop() = default;
            virtual void accept(InstructionVisitor &v) const override{ v.visit(*this); }
        };

#ifdef SCC_DROP_IMPLEMENTATION
        vm::Error Drop::execute(vm::VM &vm)
        {
            vm::Value value;
            auto error = vm.vm_stack_pop(value);
            if (error != vm::Error::None)
                return error;
            
            return vm::Error::None;
        }
#endif
    }
}