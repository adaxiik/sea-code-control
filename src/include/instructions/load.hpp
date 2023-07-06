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
        };

#ifdef SCC_LOAD_IMPLEMENTATION
        vm::Error Load::execute(vm::VM &vm)
        {
            assert(m_size_bytes <= 8 && "Load size is too big"); // for now.. do we even need more?
            uint64_t ptr{0};
            auto error = vm.vm_stack_pop(ptr);
            if (error != vm::Error::None)
                return error;

            if (ptr > vm::VM_STACK_SIZE || static_cast<int64_t>(ptr) < 0) // TODOO: ignoring heap memory for now, we'll do memory tracing later
                return vm::Error::IllegalMemoryAccess;

            uint64_t value{0};
            std::memcpy(&value, vm.get_memory() + ptr, m_size_bytes);

            error = vm.vm_stack_push(value);
            if (error != vm::Error::None)
                return error;

            return vm::Error::None;
        }
#endif
    }
}