#pragma once
#include <string>
#include "type.hpp"
#include "vm.hpp"
#include "./instruction.hpp"

namespace scc
{
    namespace instructions
    {
        struct DPrint : public Instruction // debug print instruction
        {
            virtual vm::Error execute(vm::VM &vm) override;
        };

#ifdef SCC_D_PRINT_IMPLEMENTATION
        vm::Error DPrint::execute(vm::VM &vm)
        {
            std::array<char,8> value;
            uint64_t buffer{0};

            auto error = vm.vm_stack_pop(buffer);
            if (error != vm::Error::None)
                return error;
            
            std::memcpy(&value, &buffer, 8);
            
            std::cout << "i: " << *reinterpret_cast<int*>(&value) << std::endl;
            std::cout << "ui: " << *reinterpret_cast<unsigned int*>(&value) << std::endl;
            std::cout << "l: " << *reinterpret_cast<long*>(&value) << std::endl;
            std::cout << "ul: " << *reinterpret_cast<unsigned long*>(&value) << std::endl;
            std::cout << "f: " << *reinterpret_cast<float*>(&value) << std::endl;
            std::cout << "d: " << *reinterpret_cast<double*>(&value) << std::endl;

            return vm::Error::None;

        }
#endif
    }
}