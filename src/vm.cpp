#include "vm.hpp"

#include <iostream>

namespace scc
{
    namespace vm
    {

        VM::VM(std::vector<Instruction> instructions)
            : m_stack{std::make_unique<Padding[]>(VM_STACK_SIZE)}
            , m_instructions{std::move(instructions)}
            , m_instruction_pointer{0}
            , m_stack_pointer{(VM_STACK_SIZE - 1) * Padding::Size}
        {
        }

        Error VM::execute_current()
        {
            std::visit(overloaded{[](const NewVar &new_var) 
                {
                    std::cout << "NewVar: " << new_var.name << '(' << new_var.type << ')' << (new_var.type.is_const ? " const" : "") << std::endl;
                }
            },
            m_instructions[m_instruction_pointer]);

            return Error::None;
        }

        Error VM::run()
        {
            for (; m_instruction_pointer < m_instructions.size(); m_instruction_pointer++)
            {
                auto error = execute_current();
                if (error != Error::None)
                    return error;
            }

            return Error::None;
        }

    }
}
