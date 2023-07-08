#pragma once
#include <string>
#include "type.hpp"
#include "vm.hpp"
#include "./instruction.hpp"
#include "vm/variable.hpp"

namespace scc
{
    namespace instructions
    {
        struct NewVariable : public Instruction
        {
            std::string m_name;
            type::Type m_type;

            NewVariable(std::string name, type::Type type) : m_name(name), m_type(type) {}

            virtual vm::Error execute(vm::VM &vm) override;

            virtual void accept(Visitor &v) const override{ v.visit(*this); }
        };

#ifdef SCC_NEW_VARIABLE_IMPLEMENTATION
        vm::Error NewVariable::execute(vm::VM &vm)
        {
            vm.pad_stack(m_type.size_bytes());
            vm.ref_scope_stack().create_variable(m_name, scc::vm::Variable{m_type, vm.ref_stack_pointer()});
            vm.ref_stack_pointer() -= m_type.size_bytes();
            if (vm.ref_stack_pointer() < 0)
                return vm::Error::StackOverflow;

            return vm::Error::None;
        }
#endif
    }
}