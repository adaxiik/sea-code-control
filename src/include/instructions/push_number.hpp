#pragma once
#include <string>
#include "type.hpp"
#include "vm.hpp"
#include "./instruction.hpp"

namespace scc
{
    namespace instructions
    {
        struct PushNumber : public Instruction
        {
            uint64_t m_value;
            virtual vm::Error execute(vm::VM &vm) override;

            PushNumber(uint64_t value) : m_value(value) {}
        };

#ifdef SCC_PUSH_NUMBER_IMPLEMENTATION
        vm::Error PushNumber::execute(vm::VM &vm)
        {
            auto error = vm.vm_stack_push(m_value);
            if (error != vm::Error::None)
                return error;

            return vm::Error::None;
        }
#endif
    }
}