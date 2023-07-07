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
        struct Load : public Instruction
        {
            uint8_t m_size_bytes;
            virtual vm::Error execute(vm::VM &vm) override;

            Load(uint8_t size_bytes) : m_size_bytes(size_bytes) {}

            virtual void accept(Visitor &v) const override{ v.visit(*this); }
        };

#ifdef SCC_LOAD_IMPLEMENTATION
        vm::Error Load::execute(vm::VM &vm)
        {
            assert(m_size_bytes <= 8 && "Load size is too big"); // for now.. do we even need more?
            vm::Value ptr;
            auto error = vm.vm_stack_pop(ptr);
            if (error != vm::Error::None)
                return error;

            if (ptr.as<size_t>() > vm::VM_STACK_SIZE || ptr.as<int64_t>() < 0) // TODOO: ignoring heap memory for now, we'll do memory tracing later
                return vm::Error::IllegalMemoryAccess;

            uint64_t value{0};
            std::memcpy(&value, vm.get_memory() + ptr.as<int64_t>(), m_size_bytes);

            error = vm.vm_stack_push(value);
            if (error != vm::Error::None)
                return error;

            return vm::Error::None;
        }
#endif
    }
}