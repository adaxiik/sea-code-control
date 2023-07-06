#pragma once
#include <string>
#include "type.hpp"
#include "vm.hpp"
#include "./instruction.hpp"

namespace scc
{
    namespace instructions
    {
        struct Store : public Instruction
        {
            uint8_t m_size_bytes;
            virtual vm::Error execute(vm::VM &vm) override;

            Store(uint8_t size_bytes) : m_size_bytes(size_bytes) {}
        };

#ifdef SCC_STORE_IMPLEMENTATION
        vm::Error Store::execute(vm::VM &vm)
        {
            // TODOO: check if not const somehow .. might use free bits int the pointer? because we are not going to alloc 2^64... something like nan boxing..
            uint64_t ptr{0};
            auto error = vm.vm_stack_pop(ptr);
            if (error != vm::Error::None)
                return error;

            if (ptr > vm::VM_STACK_SIZE || static_cast<int64_t>(ptr) < 0) // TODOO: ignoring heap memory for now, we'll do memory tracing later
                return vm::Error::IllegalMemoryAccess;

            uint64_t value{0};
            error = vm.vm_stack_pop(value);
            if (error != vm::Error::None)
                return error;

            std::memcpy(vm.get_memory() + ptr, &value, m_size_bytes);

            return vm::Error::None;
        }
#endif
    }
}