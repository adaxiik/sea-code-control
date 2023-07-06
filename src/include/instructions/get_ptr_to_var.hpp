#pragma once
#include <string>
#include "type.hpp"
#include "vm.hpp"
#include "./instruction.hpp"

namespace scc
{
    namespace instructions
    {
        struct GetPtrToVar : public Instruction
        {
            std::string m_name;
            GetPtrToVar(std::string name) : m_name(name) {}
            virtual vm::Error execute(vm::VM &vm) override;
        };

#ifdef SCC_GET_PTR_TO_VAR_IMPLEMENTATION
        vm::Error GetPtrToVar::execute(vm::VM &vm)
        {
            auto var = vm.ref_scope_stack()[m_name];
            if (!var.has_value())
                return vm::Error::VariableDoesNotExist;

            auto error = vm.vm_stack_push(static_cast<uint64_t>(var->pointer));
            if (error != vm::Error::None)
                return error;
            return vm::Error::None;
        }
#endif
    }
}