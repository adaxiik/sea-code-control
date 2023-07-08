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

            virtual void accept(InstructionVisitor &v) const override{ v.visit(*this); }
        };

#ifdef SCC_STORE_IMPLEMENTATION
        vm::Error Store::execute(vm::VM &vm)
        {
            // TODOO: check if not const somehow .. might use free bits int the pointer? because we are not going to alloc 2^64... something like nan boxing..
            // or check it in compile time
            vm::Value ptr;
            auto error = vm.vm_stack_pop(ptr);
            if (error != vm::Error::None)
                return error;

            if (ptr.as<size_t>() > vm::VM_STACK_SIZE || ptr.as<int64_t>() < 0) // TODOO: ignoring heap memory for now, we'll do memory tracing later
                return vm::Error::IllegalMemoryAccess;

            vm::Value value;
            error = vm.vm_stack_pop(value);
            if (error != vm::Error::None)
                return error;

            uint64_t tmp{value.as<uint64_t>()};
            std::memcpy(vm.get_memory() + ptr.as<int64_t>(), &tmp, m_size_bytes);

            return vm::Error::None;
        }
#endif
    }
}