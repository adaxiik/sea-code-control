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

            virtual void accept(InstructionVisitor &v) const override{ v.visit(*this); }

        };

#ifdef SCC_D_PRINT_IMPLEMENTATION
        vm::Error DPrint::execute(vm::VM &vm)
        {
            vm::Value value;

            auto error = vm.vm_stack_pop(value);
            if (error != vm::Error::None)
                return error;
            
            
            std::cout << "i: " << value.as<int>() << std::endl;
            std::cout << "ui: " << value.as<unsigned int>() << std::endl;
            std::cout << "l: " << value.as<long>() << std::endl;
            std::cout << "ul: " << value.as<unsigned long>() << std::endl;
            std::cout << "f: " << value.as<float>() << std::endl;
            std::cout << "d: " << value.as<double>() << std::endl;

            return vm::Error::None;

        }
#endif
    }
}