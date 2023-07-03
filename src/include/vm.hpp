#pragma once

#include <vector>
#include <memory>
#include <cstdint>
#include <map>
#include <optional>
#include <cassert>
// #include "instruction.hpp"
#include "type.hpp"
#include "vm/padding.hpp"
#include "vm/scope.hpp"
#include "vm/error.hpp"


namespace scc
{
    struct Instruction;

    namespace vm
    {
        constexpr size_t VM_STACK_SIZE = 1024 * 8; // 8 KiB
        class VM
        {
            std::unique_ptr<char[]> m_stack;
            std::vector<Instruction*> m_instructions;
            ScopeStack m_scope_stack;
            size_t m_instruction_pointer;
            int64_t m_stack_pointer;

        public:
            explicit VM(std::vector<Instruction*> instructions);

            Error execute_current();
            Error run();
            ScopeStack& ref_scope_stack() { return m_scope_stack; }
            const ScopeStack& ref_scope_stack() const { return m_scope_stack; }
            size_t& ref_instruction_pointer() { return m_instruction_pointer; }
            int64_t& ref_stack_pointer() { return m_stack_pointer; }
            char* get_stack() { return m_stack.get(); }
            const char* get_stack() const { return m_stack.get(); }

            std::vector<Instruction*> clear_instructions() 
            { 
                auto tmp = std::move(m_instructions);
                m_instructions.clear();
                return tmp;
            }

            Error pad_stack(size_t want_to_write_bytes);
            Error stack_push(uint64_t value);
            Error stack_pop(uint64_t &value);
        };

    }
}