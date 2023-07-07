#include "vm.hpp"

#include <iostream>
#include <cstring>
#include "instructions/instruction.hpp"

namespace scc
{
    namespace vm
    {

        VM::VM(std::vector<Instruction*> instructions)
            : m_memory{std::make_unique<char[]>(VM_STACK_SIZE)}
            , m_instructions{std::move(instructions)}
            , m_instruction_pointer{0}
            ,m_stack_pointer{VM_STACK_SIZE - (2 * Padding::Size)} 
        {
            m_scope_stack.push();
        }

        Error VM::execute_current()
        {
            return m_instructions[m_instruction_pointer]->execute(*this);
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

        Error VM::pad_stack(size_t want_to_write_bytes)
        {
            // TODOOOO: investigate further for bugs.. Ill check it later again, after Ill have sizeof() and structures implemented
            size_t can_fit_bytes_on_single_write = Padding::Size - (m_stack_pointer % Padding::Size);
            if (can_fit_bytes_on_single_write >= want_to_write_bytes)
                return Error::None;

            size_t needed_writes = (want_to_write_bytes / Padding::Size) + static_cast<size_t>((want_to_write_bytes % Padding::Size) != 0);

            if (needed_writes == 1) // align to 4 bytes .. for example char, int32
            {
                m_stack_pointer -= can_fit_bytes_on_single_write;
                if (m_stack_pointer < 0)
                    return Error::StackOverflow;
                return Error::None;
            }

            m_stack_pointer -= m_stack_pointer % (Padding::Size * 2); // align to 8 bytes .. for example char, int64
            if (m_stack_pointer < 0)
                return Error::StackOverflow;

            return Error::None;
        }

        Error VM::vm_stack_push(Value value)
        {
            m_vm_stack.push(std::move(value));

            return Error::None;
        }

        Error VM::vm_stack_pop(Value &value)
        {

            assert(!m_vm_stack.empty() && "VM Stack underflow");
            value = std::move(m_vm_stack.top());
            m_vm_stack.pop();

            return Error::None;
        }
    }
}
